#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "archke_error.h"

void rchkExitFailure(char* message) {
    perror(message);
    exit(1);
}

