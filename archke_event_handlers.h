#ifndef ARCHKE_SERVER_EVENT_HANDLERS
#define ARCHKE_SERVER_EVENT_HANDLERS

#define ARCHKE_WRITE_MAX_OUTPUTS 1024

#include "archke_event_loop.h"

// all functions here follow rchkHandleEvent() typedef's signature
void rchkHandleAcceptEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
void rchkHandleReadEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
void rchkHandleWriteEvent(RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);

#endif

