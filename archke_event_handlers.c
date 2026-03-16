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

// TODO: refactor
void rchkHandleWriteEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData) {
	RchkClient* client = (RchkClient*) clientData;

	RchkIovBlock iov[ARCHKE_IOV_MAX];

	int iovc = 0; // io vector element count
	RchkReplyBlock* block = client->replyRemaining;
	while (block != NULL) {
		iov[iovc].size = block->size;
		iov[iovc].buffer = block->bytes;
		iovc++;
		block = block->next;
	}

	int written = rchkSocketWritev(client->fd, iov, iovc);
	if (written < 0) {
		logError("Write to client failed");
		// close connections (exactly how it is handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}

	block = client->replyRemaining;
	while (block != NULL) {
		if (written < block->size) {
			client->replyRemaining->bytes = block->bytes + written;
			client->replyRemaining->size = block->size - written;
			client->replyRemaining->next = block->next;
			break;
		}
		written -= block->size;
		block = block->next;
	}

	// check if all the data has been sent
	if (block == NULL) {
		rchkClientReset(client);

		// register read handler for client
		RchkClientConfig config = { .data = client, .free = NULL };
		if (rchkEventLoopRegisterIOEvent(eventLoop, client->fd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleReadEvent, &config) < 0) {
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
	
	int bytes = rchkSocketRead(client->fd, client->queryBuff, client->queryBuffCap);
	if (bytes < 0) {
		logError("Read from client failed");
		// close connection (exactly how it is handled in Redis. See networking.c -> (freeClient(c) -> unlinkClient(c)))
		rchkSocketShutdown(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkSocketClose(client->fd);
		// free resources
		rchkClientFree(client);
		return;
	}
	/*
	 client sent us FIN, and we received it (client is waiting for us to send FIN back)
	 client will not send us any more data */
	if (bytes == 0) {
		// we send FIN(or possibly FIN,ACK) back (or rather we ask the kernel to send FIN back to client)
		rchkSocketShutdownWrite(client->fd);
		// then we close (release resources)
		rchkSocketClose(client->fd);
		rchkEventLoopUnregister(eventLoop, client->fd);
		rchkClientFree(client);
		return;
	}
	client->queryBuffLen = bytes;

	// pipelining
	do {
		client->queryBuffPos += rchkProcessQueryBuffer(client);

		if (!rchkIsCompleteCommandReceived(client)) {
			rchkClientResetQueryBufferState(client);
			return;
		}

		// fetch the command from the command storage
		RchkQueryArg commandName = client->argv[0];
		RchkKVValue* commandInfo = rchkKVStoreGet(server.commands, commandName.bytes, commandName.size);

		void (*command) (RchkClient*) = commandInfo->value;

		// execute command
		command(client);

		if (client->queryBuffPos < client->queryBuffLen) {
			rchkClientResetQueryParserState(client);
			rchkClientResetArgv(client);
			continue;
		}

		break;
	} while (1);

	// register write handler to send response back
	client->replyRemaining = client->reply;
	RchkClientConfig config = { .data = client, .free = NULL };

	if (rchkEventLoopRegisterIOEvent(eventLoop, client->fd, ARCHKE_EVENT_LOOP_WRITE_EVENT, rchkHandleWriteEvent, &config) < 0) {
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
	if (rchkEventLoopRegisterIOEvent(eventLoop, clientSocketFd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleReadEvent, &config) < 0) {
		rchkSocketClose(client->fd);
		rchkClientFree(client);
		logError("Client event registration failed");
	}
}
