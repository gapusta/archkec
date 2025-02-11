#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
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
	RchkResponseElement* el = client->unread;
	while (el != NULL) {
		buffs[outputs].size = el->size;
		buffs[outputs].buffer = el->bytes;
		// Great for debugging, might use later
		// for(int i=0; i<el->size; i++) {
		// 	printf("%d ", el->bytes[i]);
		// }
		outputs++;
		el = el->next;
	}
	// printf("\n");

	int read = rchkSocketWritev(client->fd, buffs, outputs);
	if (read < 0) {
		logError("Write to client failed");
		// close connections (exactly how its handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}

	el = client->unread;
	while (el != NULL) {
		if (read < el->size) {
			client->unread->bytes = el->bytes + read;
			client->unread->size = el->size - read;
			client->unread->next = el->next;
			break;
		}
		read = read - el->size;
		el = el->next;
	}

	// check if all the data has been sent
	if (el == NULL) {
		// printf("---------------------\n");
		rchkClientReset(client);

		// register read handler for client
		RchkClientConfig config = { .data = client, .free = NULL };
		if (rchkEventLoopRegister(eventLoop, client->fd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleReadEvent, &config) < 0) {
			logError("Client socket read event registration error");
			rchkSocketShutdown(client->fd);
			rchkEventLoopUnregister(eventLoop, client->fd);
			rchkSocketClose(client->fd);
			rchkClientFree(client);
			return;
		}
	}
}

void rchkHandleReadEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData) {
	RchkClient* client = (RchkClient*) clientData;
	
	int nbytes = rchkSocketRead(client->fd, client->readBuffer, client->readBufferSize);
	client->readBufferOccupied = nbytes;	

	if (nbytes < 0) {
		logError("Read from client failed");
		// close connections (exactly how its handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}

	// client sent us FIN and we received it (client is waiting for us to send FIN back)
	// client will not send us any more data
	if (nbytes == 0) {
		// we send FIN(or possibly FIN,ACK) back (or rather we ask the kernel to send FIN back to client)
		rchkSocketShutdownWrite(client->fd);
		// then we close (release resources)
		rchkSocketClose(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkClientFree(client);
		return;
	}

	do {
		int processed = rchkProcessInputQuery(client);

		if (!rchkIsProcessInputQueryDone(client)) {
			return;
		}

		// run command
		RchkKVStore* commands = getCommands();

		RchkKVValue* cmd = rchkKVStoreGet(commands, client->in[0].bytes, client->in[0].size);
		void (*command) (RchkClient*) = cmd->value;

		command(client);

		if (processed >= client->readBufferOccupied) {
			break;
		}

		rchkClientResetInputOnly(client, processed);
	} while (1);

	// register write handler to send response back
	client->unread = client->out;
	client->unreadOffset = 0;
	RchkClientConfig config = { .data = client, .free = NULL };

	if (rchkEventLoopRegister(eventLoop, client->fd, ARCHKE_EVENT_LOOP_WRITE_EVENT, rchkHandleWriteEvent, &config) < 0) {
		logError("Client socket write event registration error");
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		rchkClientFree(client);
		return;
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

