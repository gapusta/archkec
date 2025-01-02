#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_kvstore.h"

void print(const char* string, int n) {
    char out[n+1];

    memcpy(out, string, n);
    
    out[n] = '\0';
    
    printf("Hash 'get' result : %s\n", out);
}

void test1() {
    char* expected = "my-value-1";

    int keySize = strlen("my-key-1");
    int valueSize = strlen(expected);

    char* key = malloc(keySize);
    char* value = malloc(valueSize);
    if (key == NULL || value == NULL) {
        perror("malloc");
        goto clear;
    }

    memcpy(key, "key", keySize);
    memcpy(value, expected, valueSize);
    
    int code = rchkKVStorePut(key, keySize, value, valueSize);
    if (code < 0) {
        printf("Test 1 failed: 'put' returned error code: %d\n", code);
        goto clear;
    }
    
    valueSize = 0;
    value = NULL;

    code = rchkKVStoreGet(key, keySize, &value, &valueSize);
    if (code == ARCHKE_KVSTORE_NOT_FOUND) {
        printf("Test 1 failed: 'get' returned 'not found' code\n");
        goto clear;
    }
    if (code < 0) {
        printf("Test 1 failed: 'get' returned error code: %d\n", code);
        goto clear;
    }

    print(value, valueSize);

    if (strcmp(value, expected) != 0) {
        printf("Test 1 failed\n");
        goto clear;
    }

    printf("Test 1 passed\n");

clear:
    free(key);
    free(value);
}

int main(void) {
    rchkKVStoreInit();

    test1();

    return 0;
}

