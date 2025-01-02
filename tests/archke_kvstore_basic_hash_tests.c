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
    int code = rchkKVStorePut("my_key", strlen("my_key"), "my_value_2112", strlen("my_value_2112"));
    if (code < 0) {
        printf("Test 1 failed: 'put' returned error code: %d\n", code);
        return;
    }
    
    char* value = NULL;
    int valueSize = 0;

    code = rchkKVStoreGet("my_key", strlen("my_key"), &value, &valueSize);
    if (code == ARCHKE_KVSTORE_NOT_FOUND) {
        printf("Test 1 failed: 'get' returned 'not found' code\n");
        return;
    }
    if (code < 0) {
        printf("Test 1 failed: 'get' returned error code: %d\n", code);
        return;
    }

    // print(value, valueSize);

    if (valueSize != strlen("my_value_2112")) {
        printf("Test 1 failed: sizes do not match\n");
        return;
    }

    if (strcmp(value, "my_value_2112") != 0) {
        printf("Test 1 failed: contents do not match\n");
        return;
    }

    printf("Test 1 passed\n");
}

int main(void) {
    rchkKVStoreInit();

    test1();

    return 0;
}

