#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "archke_time.h"
#include "archke_event_loop.h"

RchkEventLoop* rchkEventLoopNew(int setsize) {
    RchkEventLoop* eventLoop = malloc(sizeof(RchkEventLoop));
    if (eventLoop == NULL) {
        return NULL;
    }

    eventLoop->events = NULL;
    eventLoop->apiData = NULL;

    eventLoop->events = malloc(setsize * sizeof(RchkEvent));
    if (eventLoop->events == NULL) {
        goto err;
    }

    eventLoop->apiData = malloc(setsize * sizeof(struct epoll_event));
    if (eventLoop->apiData == NULL) {
        goto err;
    }

    int epollFd = epoll_create1(0);
  	if (epollFd < 0) {
        goto err;
  	}

    eventLoop->fd = epollFd;
    eventLoop->setsize = setsize;
    
    for (int i=0; i<setsize; i++) {
        eventLoop->events[i].mask = ARCHKE_EVENT_LOOP_NONE_EVENT;
        eventLoop->events[i].readEventHandle = NULL;
        eventLoop->events[i].writeEventHandle = NULL;
        eventLoop->events[i].clientData = NULL;
        eventLoop->events[i].freeClientData = NULL;
    }

    return eventLoop;

err:
    free(eventLoop->apiData);
    free(eventLoop->events);
    free(eventLoop);

    return NULL;
}

int rchkEventLoopRegister(RchkEventLoop* eventLoop, int fd, int mask, rchkHandleEvent* proc, RchkClientConfig* config) {
    // 1. init epoll
    int epollMask = 0;
    if (mask & ARCHKE_EVENT_LOOP_READ_EVENT) epollMask |= EPOLLIN;
    if (mask & ARCHKE_EVENT_LOOP_WRITE_EVENT) epollMask |= EPOLLOUT;
    
    struct epoll_event epollEvent = { 
        .events = epollMask, 
        .data.fd = fd 
    }; 

    int op = eventLoop->events[fd].mask == ARCHKE_EVENT_LOOP_NONE_EVENT ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    int result = epoll_ctl(eventLoop->fd, op, fd, &epollEvent);
	
    if (result < 0) { 
        return -1; 
    }

    // 2. init additional event information
    RchkEvent* event = &eventLoop->events[fd];
    event->mask = mask;
    if (mask & ARCHKE_EVENT_LOOP_READ_EVENT) { event->readEventHandle = proc; }
    if (mask & ARCHKE_EVENT_LOOP_WRITE_EVENT) { event->writeEventHandle = proc; }
    event->clientData = config->data;
    event->freeClientData = config->free;

    return 0;
}

int rchkEventLoopRegisterTimeEvent(RchkEventLoop* eventLoop, long long milliseconds, rchkHandleTimeEvent* proc) {
    RchkTimeEvent* te = malloc(sizeof(RchkTimeEvent));
    if (te == NULL) {
        return -1;
    }

    te->when = getMonotonicUs() + milliseconds;
    te->eventHandle = proc;
    te->next = eventLoop->timeEventHead;

    eventLoop->timeEventHead = te;

    return 0;
}

void rchkEventLoopUnregister(RchkEventLoop* eventLoop, int fd) {
    epoll_ctl(eventLoop->fd, EPOLL_CTL_DEL, fd, NULL);

    RchkEvent* event = &eventLoop->events[fd];
    event->mask = ARCHKE_EVENT_LOOP_NONE_EVENT;
    event->readEventHandle = NULL;
    event->writeEventHandle = NULL;
    event->clientData = NULL;
    event->freeClientData = NULL;
}

static int getEarliestTimerOffset(RchkEventLoop* eventLoop) {
    RchkTimeEvent *earliest = eventLoop->timeEventHead;
    if (earliest == NULL) {
        return -1;
    }

    RchkTimeEvent *te = earliest->next;
    while (te) {
        if (te->when < earliest->when) {
            earliest = te;
        }
        te = te->next;
    }

    const uint64_t now = getMonotonicUs();
    return (now >= earliest->when) ? 0 : ((int)(earliest->when - now));
}

void rchkEventLoopMain(RchkEventLoop* eventLoop) {
    struct epoll_event* epollEvents = (struct epoll_event*) eventLoop->apiData;
	for(;;) {
	    int timeout = getEarliestTimerOffset(eventLoop);
		int nevents = epoll_wait(eventLoop->fd, epollEvents, eventLoop->setsize, timeout);
        if (nevents < 0) {
            // TODO: Handle error
            return;
        }

	    // IO events
        for (int i=0; i<nevents; i++) {
            int fd = epollEvents[i].data.fd;
            RchkEvent* event = &eventLoop->events[fd];

            if (epollEvents[i].events & EPOLLIN) {
                event->readEventHandle(eventLoop, fd, event, event->clientData);
            }

            if (epollEvents[i].events & EPOLLOUT) {
                event->writeEventHandle(eventLoop, fd, event, event->clientData);
            }
        }

        // time events
	    RchkTimeEvent* timeEvent = eventLoop->timeEventHead;
	    uint64_t now = getMonotonicUs();
	    while (timeEvent) {
	        if (now >= timeEvent->when) {
	            int milliseconds = timeEvent->eventHandle(eventLoop, timeEvent);
                now = getMonotonicUs();
	            timeEvent->when = now + milliseconds;
	        }
	        timeEvent = timeEvent->next;
	    }
    }
}

void rchkEventLoopFree(RchkEventLoop* eventLoop) {
    close(eventLoop->fd);
    free(eventLoop->apiData);

    for (int i=0; i<eventLoop->setsize; i++) {
        RchkEvent* event = &eventLoop->events[i];

        if (event->clientData != NULL && event->freeClientData != NULL) {
            event->freeClientData(event->clientData);
        }
    }

    free(eventLoop->events);
    free(eventLoop);
}

