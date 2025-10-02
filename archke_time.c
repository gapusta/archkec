#include <time.h>
#include "archke_time.h"

#include <stdio.h>

uint64_t rchkGetMonotonicUs(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec) * 1000 + ts.tv_nsec / 1000000;
}
