#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_kvstore.h"

void test1() {
    RchkKVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_value_2112", strlen("my_value_2112"));
    if (code < 0) {
        printf("Test 1 failed: 'put' returned error code: %d\n", code);
        return;
    }
    
    RchkKVValue* value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 1 failed: 'get' returned 'not found' code\n");
        return;
    }

    if (value->size != strlen("my_value_2112")) {
        printf("Test 1 failed: unexpected size\n");
        return;
    }

    if (strcmp(value->value, "my_value_2112") != 0) {
        printf("Test 1 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #1 passed\n");
}


void test2() {
    RchkKVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key_1", strlen("my_key_1"), "my_value_1", strlen("my_value_1"));
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }

    code = rchkKVStorePut(store, "my_key_2", strlen("my_key_2"), "my_value_2", strlen("my_value_2"));
    if (code < 0) {
        printf("Test 2 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }
    
    RchkKVValue* value1 = rchkKVStoreGet(store, "my_key_1", strlen("my_key_1"));
    RchkKVValue* value2 = rchkKVStoreGet(store, "my_key_2", strlen("my_key_2"));
    
    if (value1 == NULL) {
        printf("Test 2 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("my_value_1")) {
        printf("Test 2, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "my_value_1") != 0) {
        printf("Test 2, 'get' #1 failed: unexpected value\n");
        return;
    }

    if (value2 == NULL) {
        printf("Test 2 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("my_value_2")) {
        printf("Test 2, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "my_value_2") != 0) {
        printf("Test 2, 'get' #2 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #2 passed\n");
}

void test3() {
    RchkKVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_value_2112", strlen("my_value_2112"));
    if (code < 0) {
        printf("Test 3 failed: 'put' #1 returned error code: %d\n", code);
        return;
    }

    RchkKVValue* value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 3 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value->size != strlen("my_value_2112")) {
        printf("Test 3 failed: 'get' #1: unexpected size\n");
        return;
    }
    if (strcmp(value->value, "my_value_2112") != 0) {
        printf("Test 3 failed: 'get' #1: unexpected value\n");
        return;
    }

    code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_next_value_2112", strlen("my_next_value_2112"));
    if (code < 0) {
        printf("Test 3 failed: 'put' #2 returned error code: %d\n", code);
        return;
    }
    
    value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 3 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value->size != strlen("my_next_value_2112")) {
        printf("Test 3 failed: 'get' #2: unexpected size\n");
        return;
    }
    if (strcmp(value->value, "my_next_value_2112") != 0) {
        printf("Test 3 failed: 'get' #2: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #3 passed\n");
}

void test4() {
    RchkKVStore* store = rchkKVStoreNew();

    int code = rchkKVStorePut(store, "my_key", strlen("my_key"), "my_value_2112", strlen("my_value_2112"));
    if (code < 0) {
        printf("Test 4 failed: 'put' returned error code: %d\n", code);
        return;
    }
    
    RchkKVValue* value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value == NULL) {
        printf("Test 4 failed: 'get' returned 'not found' code\n");
        return;
    }

    if (value->size != strlen("my_value_2112")) {
        printf("Test 4 failed: unexpected size\n");
        return;
    }

    if (strcmp(value->value, "my_value_2112") != 0) {
        printf("Test 4 failed: unexpected value\n");
        return;
    }

    rchkKVStoreDelete(store, "my_key", strlen("my_key"));

    value = rchkKVStoreGet(store, "my_key", strlen("my_key"));
    if (value != NULL) {
        printf("Test 4 failed: delete failed\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #4 passed\n");
}

static uint64_t rchkHashTestSameHash(const char* target, int targetSize) {
    return 0;
}

void test5() {
    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    
    if (rchkKVStorePut(store, "k1", strlen("k1"), "v1", strlen("v1")) < 0) {
        printf("Test 5 failed: 'put' #1 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k2", strlen("k2"), "v2", strlen("v2")) < 0) {
        printf("Test 5 failed: 'put' #2 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k3", strlen("k3"), "v3", strlen("v3")) < 0) {
        printf("Test 5 failed: 'put' #3 failed\n");
        return;
    }

    RchkKVValue* value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 5 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 5, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 5, 'get' #1 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 5 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 5, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 5, 'get' #2 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 5 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 5, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 5, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #5 passed\n");
}

void test6() {
    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    
    if (rchkKVStorePut(store, "k1", strlen("k1"), "v1", strlen("v1")) < 0) {
        printf("Test 6 failed: 'put' #1 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k2", strlen("k2"), "v2", strlen("v2")) < 0) {
        printf("Test 6 failed: 'put' #2 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k3", strlen("k3"), "v3", strlen("v3")) < 0) {
        printf("Test 6 failed: 'put' #3 failed\n");
        return;
    }

    RchkKVValue* value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 6 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 6, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 6, 'get' #1 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 6 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 6, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 6, 'get' #2 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 6 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 6, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 6, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreDelete(store, "k2", strlen("k2"));

    value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 != NULL) {
        printf("Test 6 failed: 'delete' failed\n");
        return;
    }

    value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 6 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 6, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 6, 'get' #1 failed: unexpected value\n");
        return;
    }
    
    value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 6 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 6, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 6, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #6 passed\n");
}

void test7() {
    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    
    if (rchkKVStorePut(store, "k1", strlen("k1"), "v1", strlen("v1")) < 0) {
        printf("Test 7 failed: 'put' #1 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k2", strlen("k2"), "v2", strlen("v2")) < 0) {
        printf("Test 7 failed: 'put' #2 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k3", strlen("k3"), "v3", strlen("v3")) < 0) {
        printf("Test 7 failed: 'put' #3 failed\n");
        return;
    }

    RchkKVValue* value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 7 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 7, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 7, 'get' #1 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 7 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 7, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 7, 'get' #2 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 7 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 7, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 7, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreDelete(store, "k1", strlen("k1"));

    value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 != NULL) {
        printf("Test 7 failed: 'delete' failed\n");
        return;
    }

    value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 7 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 7, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 7, 'get' #2 failed: unexpected value\n");
        return;
    }

    value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 7 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 7, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 7, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #7 passed\n");
}

void test8() {
    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    
    if (rchkKVStorePut(store, "k1", strlen("k1"), "v1", strlen("v1")) < 0) {
        printf("Test 8 failed: 'put' #1 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k2", strlen("k2"), "v2", strlen("v2")) < 0) {
        printf("Test 8 failed: 'put' #2 failed\n");
        return;
    }
    if (rchkKVStorePut(store, "k3", strlen("k3"), "v3", strlen("v3")) < 0) {
        printf("Test 8 failed: 'put' #3 failed\n");
        return;
    }

    RchkKVValue* value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 8 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 8, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 8, 'get' #1 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 8 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 8, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 8, 'get' #2 failed: unexpected value\n");
        return;
    }

    RchkKVValue* value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 == NULL) {
        printf("Test 8 failed: 'get' #3 returned 'not found' code\n");
        return;
    }
    if (value3->size != strlen("v3")) {
        printf("Test 8, 'get' #3 failed: unexpected size\n");
        return;
    }
    if (strcmp(value3->value, "v3") != 0) {
        printf("Test 8, 'get' #3 failed: unexpected value\n");
        return;
    }

    rchkKVStoreDelete(store, "k3", strlen("k3"));

    value3 = rchkKVStoreGet(store, "k3", strlen("k3"));
    if (value3 != NULL) {
        printf("Test 8 failed: 'delete' failed\n");
        return;
    }

    value1 = rchkKVStoreGet(store, "k1", strlen("k1"));
    if (value1 == NULL) {
        printf("Test 8 failed: 'get' #1 returned 'not found' code\n");
        return;
    }
    if (value1->size != strlen("v1")) {
        printf("Test 8, 'get' #1 failed: unexpected size\n");
        return;
    }
    if (strcmp(value1->value, "v1") != 0) {
        printf("Test 8, 'get' #1 failed: unexpected value\n");
        return;
    }

    value2 = rchkKVStoreGet(store, "k2", strlen("k2"));
    if (value2 == NULL) {
        printf("Test 8 failed: 'get' #2 returned 'not found' code\n");
        return;
    }
    if (value2->size != strlen("v2")) {
        printf("Test 8, 'get' #2 failed: unexpected size\n");
        return;
    }
    if (strcmp(value2->value, "v2") != 0) {
        printf("Test 8, 'get' #2 failed: unexpected value\n");
        return;
    }

    rchkKVStoreFree(store);

    printf("Test #8 passed\n");
}

int main(void) {
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    return 0;
}

