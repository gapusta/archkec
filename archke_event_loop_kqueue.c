#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "archke_event_loop.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "archke_error.h"
#include "archke_time.h"

static int kqueueUnregisterEvent(int eventLoopFd, int fromFd, RchkEvent* registeredEvent) {
    struct kevent prevEvent;
    if (registeredEvent->mask & ARCHKE_EVENT_LOOP_READ_EVENT) {
        EV_SET(&prevEvent, fromFd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    }
    if (registeredEvent->mask & ARCHKE_EVENT_LOOP_WRITE_EVENT) {
        EV_SET(&prevEvent, fromFd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    }
    return kevent(eventLoopFd, &prevEvent, 1, NULL, 0, NULL);
}

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

    eventLoop->apiData = malloc(setsize * sizeof(struct kevent));
    if (eventLoop->apiData == NULL) {
        goto err;
    }

    int kq = kqueue();
    if (kq < 0) {
        goto err;
    }

    eventLoop->fd = kq;
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

int  rchkEventLoopRegisterIOEvent(RchkEventLoop* eventLoop, int fd, int mask, rchkHandleEvent* proc, RchkClientConfig* config) {
    RchkEvent* event = &eventLoop->events[fd];

    // 1. Remove previously listened event if required
    if (event->mask != ARCHKE_EVENT_LOOP_NONE_EVENT) {
        int result = kqueueUnregisterEvent(eventLoop->fd, fd, event);
        if (result < 0) {
            return -1;
        }
    }

    // 2. register new event to listen
    struct kevent newEvent;
    if (mask & ARCHKE_EVENT_LOOP_READ_EVENT) {
        EV_SET(&newEvent, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    }
    if (mask & ARCHKE_EVENT_LOOP_WRITE_EVENT) {
        EV_SET(&newEvent, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    }
    if (kevent(eventLoop->fd, &newEvent, 1, NULL, 0, NULL) < 0) {
        return -1;
    }

    // 3. init additional event information
    event->mask = mask;
    if (mask & ARCHKE_EVENT_LOOP_READ_EVENT) { event->readEventHandle = proc; }
    if (mask & ARCHKE_EVENT_LOOP_WRITE_EVENT) { event->writeEventHandle = proc; }
    event->clientData = config->data;
    event->freeClientData = config->free;

    return 0;
}

int  rchkEventLoopRegisterTimeEvent(RchkEventLoop* eventLoop, long long milliseconds, rchkHandleTimeEvent* proc) {
    RchkTimeEvent* te = malloc(sizeof(RchkTimeEvent));
    if (te == NULL) {
        return -1;
    }

    te->when = rchkGetMonotonicUs() + milliseconds;
    te->eventHandle = proc;
    te->next = eventLoop->timeEventHead;

    eventLoop->timeEventHead = te;

    return 0;
}

void rchkEventLoopUnregister(RchkEventLoop* eventLoop, int fd) {
    RchkEvent* event = &eventLoop->events[fd];

    kqueueUnregisterEvent(eventLoop->fd, fd, event);

    event->mask = ARCHKE_EVENT_LOOP_NONE_EVENT;
    event->readEventHandle = NULL;
    event->writeEventHandle = NULL;
    event->clientData = NULL;
    event->freeClientData = NULL;
}

static int setEarliestTimerOffset(RchkEventLoop* eventLoop, struct timespec* timespec) {
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

    const uint64_t now = rchkGetMonotonicUs();
    if (now >= earliest->when) {
        timespec->tv_sec = 0;
        timespec->tv_nsec = 0;
    } else {
        int delta = earliest->when - now;
        timespec->tv_sec = delta / 1000;
        timespec->tv_nsec = (delta % 1000) * 1000000;
    }

    return 0;
}

void rchkEventLoopMain(RchkEventLoop* eventLoop) {
    struct kevent* kqueueEvents = (struct kevent*) eventLoop->apiData;
    for(;;) {
        struct timespec timeout;
        int timeset = setEarliestTimerOffset(eventLoop, &timeout);
        int nevents = kevent(eventLoop->fd, NULL, 0, kqueueEvents, eventLoop->setsize, timeset == 0 ? &timeout : NULL);
        if (nevents == -1 && errno != EINTR) {
            rchkExitFailure("polling: kqueue wait");
            return;
        }

        // IO events
        for (int i=0; i<nevents; i++) {
            int fd = kqueueEvents[i].ident;
            RchkEvent* event = &eventLoop->events[fd];

            if (kqueueEvents[i].filter == EVFILT_READ) {
                event->readEventHandle(eventLoop, fd, event, event->clientData);
            }

            if (kqueueEvents[i].filter == EVFILT_WRITE) {
                event->writeEventHandle(eventLoop, fd, event, event->clientData);
            }
        }

        // Time events
        RchkTimeEvent* timeEvent = eventLoop->timeEventHead;
        uint64_t now = rchkGetMonotonicUs();
        while (timeEvent) {
            if (now >= timeEvent->when) {
                int milliseconds = timeEvent->eventHandle(eventLoop, timeEvent);
                now = rchkGetMonotonicUs();
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
