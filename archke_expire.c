#include "archke_expire.h"

#include <stdlib.h>

#include "archke_server.h"
#include "archke_kvstore.h"
#include "archke_error.h"
#include "archke_memory.h"
#include "archke_time.h"

/*
    Helper function. Used when key deletion happens to free memory expiration date
*/
static void rchkDelExpireValue(char* key, int keySize, void* value, int valueSize) {
    rchkFreeDuplicate(key, keySize);
    free(value);
}

int rchkSetExpireTime(char* key, int keySize, uint64_t timeout) {
    uint64_t* when = malloc(sizeof(uint64_t));
    if (when == NULL) {
        rchkExitFailure("PANIC: memory allocation failed");
    }
    char* keyDup = rchkDuplicate(key, keySize);
    uint64_t now = getMonotonicUs();

    *when = now + timeout;

    if (rchkKVStorePut(server.expire, keyDup, keySize, when, -1) < 0) {
        // TODO: write better error error handling
        rchkExitFailure("'set' operation failed");
    }

    return 0;
}

void rchkRemoveExpireTime(char* key, int keySize) {
    rchkKVStoreDelete2(server.expire, key, keySize, rchkDelExpireValue);
}

int rchkIsExpired(char* key, int keySize) {
    RchkKVValue* el = rchkKVStoreGet(server.expire, key, keySize);
    if (el == NULL) {
        return 0;
    }
    uint64_t now = getMonotonicUs();
    uint64_t* when = el->value;

    if (now > *when) {
        return 1;
    }
    return 0;
}
