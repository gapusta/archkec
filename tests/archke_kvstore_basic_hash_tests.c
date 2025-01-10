#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_kvstore.h"

void test1() {
    KVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_value_2112");
    if (code < 0) {
        printf("Test 1 failed: 'put' returned error code: %d\n", code);
        return;
    }
    
    void* value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 1 failed: 'get' returned 'not found' code\n");
        return;
    }

    if (strcmp(value, "my_value_2112") != 0) {
        printf("Test 1 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test 1 passed\n");
}


void test2() {
    KVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key_1", strlen("my_key_1"), "my_value_1");
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }

    code = rchkKVStorePut(store, "my_key_2", strlen("my_key_2"), "my_value_2");
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }
    
    void* value1 = rchkKVStoreGet(store, "my_key_1", strlen("my_key_1"));
    void* value2 = rchkKVStoreGet(store, "my_key_2", strlen("my_key_2"));
    
    if (value1 == NULL) {
        printf("Test 2 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value2 == NULL) {
        printf("Test 2 failed: 'get' #2 returned 'not found' code\n");
        return;
    }

    if (strcmp(value1, "my_value_1") != 0) {
        printf("Test 2, 'get' #1 failed: unexpected value\n");
        return;
    }
    if (strcmp(value2, "my_value_2") != 0) {
        printf("Test 2, 'get' #2 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test 2 passed\n");
}

void test3() {
    KVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_value_2112");
    if (code < 0) {
        printf("Test 3 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }

    void* value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 3 failed: 'get' #1 returned 'not found' code\n");
        return;
    }

    if (strcmp(value, "my_value_2112") != 0) {
        printf("Test 3 failed: 'get' #1: unexpected value\n");
        return;
    }

    code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_next_value_2112");
    if (code < 0) {
        printf("Test 3 failed: 'put' #2 returned error code: %d\n", code);
        return;
    }
    
    value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 3 failed: 'get' #2 returned 'not found' code\n");
        return;
    }

    if (strcmp(value, "my_next_value_2112") != 0) {
        printf("Test 3 failed: 'get' #2: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test 3 passed\n");
}

int main(void) {
    test1();
    test2();
    test3();
    return 0;
}

