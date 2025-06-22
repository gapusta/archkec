#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "archke_server.h"
#include "archke_socket.h"
#include "archke_event_loop.h"
#include "archke_event_handlers.h"
#include "archke_logs.h"
#include "archke_commands.h"

void rchkHandleWriteEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData) {
	RchkClient* client = (RchkClient*) clientData;

	RchkSocketBuffer buffs[ARCHKE_WRITE_MAX_OUTPUTS];

	int outputs = 0;
	RchkResponseElement* element = client->unwritten;
	while (element != NULL) {
		buffs[outputs].size = element->size;
		buffs[outputs].buffer = element->bytes;
		outputs++;
		element = element->next;
	}

	int bytesWrittenAmount = rchkSocketWritev(client->fd, buffs, outputs);
	if (bytesWrittenAmount < 0) {
		logError("Write to client failed");
		// close connections (exactly how it is handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}

	element = client->unwritten;
	while (element != NULL) {
		if (bytesWrittenAmount < element->size) {
			client->unwritten->bytes = element->bytes + bytesWrittenAmount;
			client->unwritten->size = element->size - bytesWrittenAmount;
			client->unwritten->next = element->next;
			break;
		}
		bytesWrittenAmount -= element->size;
		element = element->next;
	}

	// check if all the data has been sent
	if (element == NULL) {
		rchkClientReset(client);

		// register read handler for client
		RchkClientConfig config = { .data = client, .free = NULL };
		if (rchkEventLoopRegister(eventLoop, client->fd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleReadEvent, &config) < 0) {
			logError("Client socket read event registration error");
			rchkSocketShutdown(client->fd);
			rchkEventLoopUnregister(eventLoop, client->fd);
			rchkSocketClose(client->fd);
			rchkClientFree(client);
		}
	}
}

void rchkHandleReadEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData) {
	RchkClient* client = (RchkClient*) clientData;
	
	int bytesReceivedAmount = rchkSocketRead(client->fd, client->readBuffer, client->readBufferSize);
	client->readBufferOccupied = bytesReceivedAmount;

	if (bytesReceivedAmount < 0) {
		logError("Read from client failed");
		// close connections (exactly how it is handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}

	// client sent us FIN, and we received it (client is waiting for us to send FIN back)
	// client will not send us any more data
	if (bytesReceivedAmount == 0) {
		// we send FIN(or possibly FIN,ACK) back (or rather we ask the kernel to send FIN back to client)
		rchkSocketShutdownWrite(client->fd);
		// then we close (release resources)
		rchkSocketClose(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkClientFree(client);
		return;
	}

	// pipelining
	do {
		int processedBytesAmount = rchkProcessReadBuffer(client);

		if (!rchkIsCompleteCommandReceived(client)) {
			return;
		}

		// fetch the command from command store
		RchkKVStore* commands = getCommands();

		char* commandName = client->commandElements[0].bytes;
		int commandNameSize = client->commandElements[0].size;

		RchkKVValue* cmd = rchkKVStoreGet(commands, commandName, commandNameSize);

		void (*command) (RchkClient*) = cmd->value;

		// run command
		command(client);

		if (processedBytesAmount < client->readBufferOccupied) {
			rchkClientResetInputOnly(client, processedBytesAmount);
			continue;
		}

		break;
	} while (1);

	// register write handler to send response back
	client->unwritten = client->out;
	RchkClientConfig config = { .data = client, .free = NULL };

	if (rchkEventLoopRegister(eventLoop, client->fd, ARCHKE_EVENT_LOOP_WRITE_EVENT, rchkHandleWriteEvent, &config) < 0) {
		logError("Client socket write event registration error");
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		rchkClientFree(client);
	}
}

void rchkHandleAcceptEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData) {
	int serverSocketFd = *((int*)clientData);

	int clientSocketFd = rchkServerSocketAccept(serverSocketFd);
	if (clientSocketFd < 0) {
		logError("Accept client connection failed"); 
		return;
	}

	if (rchkSocketSetMode(clientSocketFd, ARCHKE_SOCKET_MODE_NON_BLOCKING) < 0) {
		rchkSocketClose(clientSocketFd);
		logError("Make client socket non-blocking failed");
		return;
	}

	// initialize client data
	RchkClient* client = rchkClientNew(clientSocketFd);
	if (client == NULL) {
		rchkSocketClose(clientSocketFd);
		logError("Client data init failed");
		return;
	}
	
	RchkClientConfig config = { 
		.data = client, 
		.free = NULL 
	};
	// register read handler for new client. TODO: implement 'free()' 
	if (rchkEventLoopRegister(eventLoop, clientSocketFd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleReadEvent, &config) < 0) {
		rchkSocketClose(client->fd);
		rchkClientFree(client);
		logError("Client event registration failed");
	}
}
