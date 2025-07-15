#include <stdio.h>
#include <stdlib.h>
#include "archke_error.h"

void rchkExitFailure(const char* message) {
    perror(message); // TODO: consider using strerror(errno) for more redability
    exit(1);
}

