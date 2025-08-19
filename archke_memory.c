#include <stdlib.h>
#include <string.h>
#include "archke_memory.h"
#include "archke_error.h"

char* rchkDuplicate(const char* bytes, int size) {
    void* dup = malloc(size);
    if (dup == NULL) {
        // TODO: write better error error handling
        rchkExitFailure("duplication operation failed: malloc");
    }
    memcpy(dup, bytes, size);
    return dup;
}

void rchkFreeDuplicate(char* bytes, int size) {
    free(bytes);
}

/*
    Helper function. Used when key deletion happens to free memory from key and value
*/
void rchkDelFreeKeyValue(char* key, int keySize, void* value, int valueSize) {
    rchkFreeDuplicate(key, keySize);
    rchkFreeDuplicate(value, valueSize);
}
