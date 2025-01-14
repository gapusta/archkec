#ifndef ARCHKE_ECHO_SERVER_EVENT_HANDLERS
#define ARCHKE_ECHO_SERVER_EVENT_HANDLERS

#include "archke_event_loop.h"
#include "archke_binary_string_array_reader.h"

/*
typedef struct RchkArrayElement {
	char* bytes;
	int size;
} RchkArrayElement;

typedef struct RchkClient {
	int fd;

	int readState;	
	char* readBuffer;
	int readBufferSize;
	int readBufferPos;

    RchkArrayElement* in;
	int inCount;
	int inIndex;
	
	char* writeBuffer;
	int writeBufferSize;
	int writeBufferPos;
    
	RchkArrayElement* out;
	int outCount;
	int outIndex;
} RchkClient;

// all functions here follow rchkHandleEvent() typedef's signature
void rchkHandleAcceptEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
void rchkHandleReadEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
void rchkHandleWriteEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
*/

#endif

