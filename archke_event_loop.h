#ifndef ARCHKE_EVENT_LOOP
#define ARCHKE_EVENT_LOOP

#include <stdint.h>

#define ARCHKE_EVENT_LOOP_NONE_EVENT  0
#define ARCHKE_EVENT_LOOP_READ_EVENT  1
#define ARCHKE_EVENT_LOOP_WRITE_EVENT 2

struct RchkEventLoop;
struct RchkEvent;
struct RchkTimeEvent;

typedef void rchkHandleEvent(struct RchkEventLoop* eventLoop, int fd, struct RchkEvent* event, void* clientData);
typedef int rchkHandleTimeEvent(struct RchkEventLoop* eventLoop, struct RchkTimeEvent* event);
typedef void freeClientData(void* clientData);

typedef struct RchkClientConfig {
    void* data;
    freeClientData* free;
} RchkClientConfig;

typedef struct RchkEvent {
    int mask;
    rchkHandleEvent* readEventHandle;
    rchkHandleEvent* writeEventHandle;
    void* clientData;
    freeClientData* freeClientData;
} RchkEvent;

typedef struct RchkTimeEvent {
    uint64_t when;
    rchkHandleTimeEvent* eventHandle;
    struct RchkTimeEvent* next;
} RchkTimeEvent;

typedef struct RchkEventLoop {
    int fd;
    int setsize; /* max number of file descriptors tracked */
    RchkEvent* events; /* registered events */
    RchkTimeEvent* timeEventHead;
    void* apiData; /* This is used for polling API specific data */
} RchkEventLoop;

RchkEventLoop* rchkEventLoopNew(int setsize);

/*
 * Register an event. Previously registered events will be erased
 */
int  rchkEventLoopRegisterIOEvent(RchkEventLoop* eventLoop, int fd, int mask, rchkHandleEvent* proc, RchkClientConfig* config);

int  rchkEventLoopRegisterTimeEvent(RchkEventLoop* eventLoop, long long milliseconds, rchkHandleTimeEvent* proc);
void rchkEventLoopMain(RchkEventLoop* eventLoop); // main event loop
void rchkEventLoopUnregister(RchkEventLoop* eventLoop, int fd);
void rchkEventLoopFree(RchkEventLoop* eventLoop);

#endif

