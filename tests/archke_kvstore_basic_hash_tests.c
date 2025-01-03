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

void test2() {
    int code = rchkKVStorePut("my_key_1", strlen("my_key_1"), "my_value_1", strlen("my_value_1"));
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }

    code = rchkKVStorePut("my_key_2", strlen("my_key_2"), "my_value_2", strlen("my_value_2"));
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }
    
    char* value = NULL;
    int valueSize = 0;

    code = rchkKVStoreGet("my_key_1", strlen("my_key_1"), &value, &valueSize);
    if (code == ARCHKE_KVSTORE_NOT_FOUND) {
        printf("Test 2 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (code < 0) {
        printf("Test 2 failed: 'get' #1 returned error code: %d\n", code);
        return;
    }

    if (valueSize != strlen("my_value_1")) {
        printf("Test 2, 'get' #1 failed: sizes do not match\n");
        return;
    }

    if (strcmp(value, "my_value_1") != 0) {
        printf("Test 2, 'get' #1 failed: contents do not match\n");
        return;
    }

    // print(value, valueSize);

    value = NULL;
    valueSize = 0;

    code = rchkKVStoreGet("my_key_2", strlen("my_key_2"), &value, &valueSize);
    if (code == ARCHKE_KVSTORE_NOT_FOUND) {
        printf("Test 2 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (code < 0) {
        printf("Test 2 failed: 'get' #2 returned error code: %d\n", code);
        return;
    }

    if (valueSize != strlen("my_value_2")) {
        printf("Test 2, 'get' #2 failed: sizes do not match\n");
        return;
    }

    if (strcmp(value, "my_value_2") != 0) {
        printf("Test 2, 'get' #2 failed: contents do not match\n");
        return;
    }

    // print(value, valueSize);

    printf("Test 2 passed\n");
}

void test3() {
    int code = rchkKVStorePut("my_key", strlen("my_key"), "my_value_2112", strlen("my_value_2112"));
    if (code < 0) {
        printf("Test 3 failed: 'put' returned error code: %d\n", code);
        return;
    }

    code = rchkKVStorePut("my_key", strlen("my_key"), "my_next_value_2112", strlen("my_next_value_2112"));
    if (code < 0) {
        printf("Test 3 failed: 'put' returned error code: %d\n", code);
        return;
    }
    
    char* value = NULL;
    int valueSize = 0;

    code = rchkKVStoreGet("my_key", strlen("my_key"), &value, &valueSize);
    if (code == ARCHKE_KVSTORE_NOT_FOUND) {
        printf("Test 3 failed: 'get' returned 'not found' code\n");
        return;
    }
    if (code < 0) {
        printf("Test 3 failed: 'get' returned error code: %d\n", code);
        return;
    }

    // print(value, valueSize);

    if (valueSize != strlen("my_next_value_2112")) {
        printf("Test 3 failed: sizes do not match\n");
        return;
    }

    if (strcmp(value, "my_next_value_2112") != 0) {
        printf("Test 3 failed: contents do not match\n");
        return;
    }

    printf("Test 3 passed\n");
}

int main(void) {
    rchkKVStoreInit();

    test1();
    test2();
    test3();

    return 0;
}

