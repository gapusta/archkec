#include <stdlib.h>
#include "archke_commands.h"
#include "archke_error.h"
#include "archke_socket.h"
#include "archke_event_loop.h"
#include "archke_event_handlers.h"
#include "archke_expire.h"

int main(void) {
	// server initialization
	rchkServerInit();
	initKvstore();
	initCommands();

	// create socket, open it and make it listen on port
	int serverSocketFd = rchkServerSocketNew(9999);
	if (serverSocketFd < 0) {
		rchkExitFailure("Cannot create/init server socket failed");
	}

	// set server socket IO to non-blocking mode
	if (rchkSocketSetMode(serverSocketFd, ARCHKE_SOCKET_MODE_NON_BLOCKING) < 0) {
		rchkServerSocketClose(serverSocketFd);
		rchkExitFailure("Cannot make server socket non-blocking");
	}

	// create the event loop
	RchkEventLoop* eventLoop = rchkEventLoopNew(512);
  	if (eventLoop == NULL) {
		rchkServerSocketClose(serverSocketFd);
		rchkExitFailure("Cannot create/init main event loop");
  	}

	// register server cron as time event
	if (rchkEventLoopRegisterTimeEvent(eventLoop, 1, serverCron) < 0) {
		rchkServerSocketClose(serverSocketFd);
		rchkExitFailure("Cannot register 'server cron' time event");
	}

	// register server's socket and "accept" event handler
	RchkClientConfig config = { .data = &serverSocketFd, .free = NULL };
	if (rchkEventLoopRegisterIOEvent(eventLoop, serverSocketFd, ARCHKE_EVENT_LOOP_READ_EVENT, rchkHandleAcceptEvent, &config) < 0) {
		rchkEventLoopFree(eventLoop);
		rchkServerSocketClose(serverSocketFd);
		rchkExitFailure("Cannot register 'connection accept' event handler");
	}

	// run event loop
	rchkEventLoopMain(eventLoop);

	rchkEventLoopFree(eventLoop);
	rchkServerSocketClose(serverSocketFd);

	return 0;
}

