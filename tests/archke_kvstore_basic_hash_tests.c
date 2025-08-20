#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../archke_tests.h"
#include "../archke_kvstore.h"

void test1() {
    rchkTestSetName("Test #1");

    RchkKVStore* store = rchkKVStoreNew();
    rchkAssertNotNull(store, "store");

    char* key = "key";
    char* value = "value";
    
    // 1. put
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key, strlen(key), value, strlen(value)), "put");
    
    // 2. get
    RchkKVValue* result = rchkKVStoreGet(store, key, strlen(key));
    rchkAssertNotNull(result, "result");
    rchkAssertEqualsInt(strlen(value), result->size, "result");
    rchkAssertEqualsContent(value, result->value, strlen(value), "result");

    rchkKVStoreFree(store);

    printf("Test #1 passed\n");
}

void test2() {
    rchkTestSetName("Test #2");

    RchkKVStore* store = rchkKVStoreNew();
    rchkAssertNotNull(store, "store");

    char* key1 = "key";
    char* value1 = "value";

    char* key2 = "key2";
    char* value2 = "value2";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1"); // put 1
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2"); // put 2

    // get 1
    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1)); 
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    // get 2
    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    rchkKVStoreFree(store);

    printf("Test #2 passed\n");
}

void test3() {
    rchkTestSetName("Test #3");

    RchkKVStore* store = rchkKVStoreNew();
    rchkAssertNotNull(store, "store");

    char* key1 = "key";
    char* value1 = "value";

    char* key2 = "key2";
    char* value2 = "value2";

    // put 1
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");

    // get 1
    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    // put 2
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");

    // get 2
    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    rchkKVStoreFree(store);

    printf("Test #3 passed\n");
}

void test4() {
    rchkTestSetName("Test #4");

    RchkKVStore* store = rchkKVStoreNew();
    rchkAssertNotNull(store, "store");

    char* key = "key";
    char* value = "value";
    
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key, strlen(key), value, strlen(value)), "put");
    
    RchkKVValue* resultBefore = rchkKVStoreGet(store, key, strlen(key));
    rchkAssertNotNull(resultBefore, "resultBefore");
    rchkAssertEqualsInt(strlen(value), resultBefore->size, "resultBefore");
    rchkAssertEqualsContent(value, resultBefore->value, strlen(value), "resultBefore");

    // delete
    rchkKVStoreDelete(store, key, strlen(key));

    RchkKVValue* resultAfter = rchkKVStoreGet(store, key, strlen(key));
    rchkAssertNull(resultAfter, "resultAfter");

    rchkKVStoreFree(store);

    printf("Test #4 passed\n");
}

static uint64_t rchkHashTestSameHash(const char* target, int targetSize) {
    return 0;
}

void test5() {
    rchkTestSetName("Test #5");

    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    rchkAssertNotNull(store, "store");
    
    char* key1 = "k1";
    char* value1 = "v1";

    char* key2 = "k2";
    char* value2 = "v2";

    char* key3 = "k3";
    char* value3 = "v3";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key3, strlen(key3), value3, strlen(value3)), "put3");

    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    RchkKVValue* result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3");

    rchkKVStoreFree(store);

    printf("Test #5 passed\n");
}

void test6() {
    rchkTestSetName("Test #6");

    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    rchkAssertNotNull(store, "store");

    char* key1 = "k1";
    char* value1 = "v1";

    char* key2 = "k2";
    char* value2 = "v2";

    char* key3 = "k3";
    char* value3 = "v3";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key3, strlen(key3), value3, strlen(value3)), "put3");

    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    RchkKVValue* result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3");

    // delete
    rchkKVStoreDelete(store, key2, strlen(key2));

    result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1 after");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1 after");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1 after");

    // check delete
    result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNull(result2, "result2 after");

    result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3 after");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3 after");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3 after");

    rchkKVStoreFree(store);

    printf("Test #6 passed\n");
}

void test7() {
    rchkTestSetName("Test #7");

    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    rchkAssertNotNull(store, "store");

    char* key1 = "k1";
    char* value1 = "v1";

    char* key2 = "k2";
    char* value2 = "v2";

    char* key3 = "k3";
    char* value3 = "v3";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key3, strlen(key3), value3, strlen(value3)), "put3");

    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    RchkKVValue* result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3");

    // delete
    rchkKVStoreDelete(store, key1, strlen(key1));

    // check delete
    result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNull(result1, "result1 after");

    result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(value2, "result2 after");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2 after");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2 after");

    result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3 after");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3 after");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3 after");

    rchkKVStoreFree(store);

    printf("Test #7 passed\n");
}

void test8() {
    rchkTestSetName("Test #8");

    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);
    rchkAssertNotNull(store, "store");

    char* key1 = "k1";
    char* value1 = "v1";

    char* key2 = "k2";
    char* value2 = "v2";

    char* key3 = "k3";
    char* value3 = "v3";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key3, strlen(key3), value3, strlen(value3)), "put3");

    RchkKVValue* result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(result1, "result1");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1");

    RchkKVValue* result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(result2, "result2");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2");

    RchkKVValue* result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNotNull(result3, "result3");
    rchkAssertEqualsInt(strlen(value3), result3->size, "result3");
    rchkAssertEqualsContent(value3, result3->value, strlen(value3), "result3");

    // delete
    rchkKVStoreDelete(store, key3, strlen(key3));
    
    result1 = rchkKVStoreGet(store, key1, strlen(key1));
    rchkAssertNotNull(value1, "result1 after");
    rchkAssertEqualsInt(strlen(value1), result1->size, "result1 after");
    rchkAssertEqualsContent(value1, result1->value, strlen(value1), "result1 after");

    result2 = rchkKVStoreGet(store, key2, strlen(key2));
    rchkAssertNotNull(value2, "result2 after");
    rchkAssertEqualsInt(strlen(value2), result2->size, "result2 after");
    rchkAssertEqualsContent(value2, result2->value, strlen(value2), "result2 after");

    // check delete
    result3 = rchkKVStoreGet(store, key3, strlen(key3));
    rchkAssertNull(result3, "result3 after");

    rchkKVStoreFree(store);

    printf("Test #8 passed\n");
}

void test9() {
    rchkTestSetName("Test #9");

    RchkKVStore* store = rchkKVStoreNew2(rchkHashTestSameHash);

    rchkAssertNotNull(store, "store");

    char* key1 = "k1"; char* value1 = "v1";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");

    RchkKVStoreScanner* scanner = rchkKVStoreScanNew(store);

    rchkAssertNotNull(scanner, "scanner");

    RchkKVKeyValue current;

    while (!rchkKVStoreScanIsDone(scanner)) {
        rchkKVStoreScanGet(scanner, &current);
        char* key = current.key;
        char* value = current.value;

        rchkAssertEqualsContent("k1", key, strlen("k1"), "key check");
        rchkAssertEqualsContent("v1", value, strlen("v1"), "value check");

        rchkKVStoreScanMove(scanner);
    }

    rchkKVStoreScanFree(scanner);
    rchkKVStoreFree(store);

    printf("Test #9 passed\n");
}

static uint64_t testHash(const char* target, int targetSize) {
    if (strcmp(target, "k1") == 0) return 0;
    if (strcmp(target, "k2") == 0) return 1;
    if (strcmp(target, "k3") == 0) return 2;
    if (strcmp(target, "k4") == 0) return 0;
    if (strcmp(target, "k5") == 0) return 1;
    if (strcmp(target, "k6") == 0) return 2;

    return 3;
}

void test10() {
    rchkTestSetName("Test #10");

    RchkKVStore* store = rchkKVStoreNew2(testHash);

    rchkAssertNotNull(store, "store");

    char* key1 = "k1"; char* value1 = "v1";
    char* key2 = "k2"; char* value2 = "v2";
    char* key3 = "k3"; char* value3 = "v3";
    char* key4 = "k4"; char* value4 = "v4";
    char* key5 = "k5"; char* value5 = "v5";
    char* key6 = "k6"; char* value6 = "v6";

    rchkAssertEqualsInt(0, rchkKVStorePut(store, key1, strlen(key1), value1, strlen(value1)), "put1");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key2, strlen(key2), value2, strlen(value2)), "put2");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key3, strlen(key3), value3, strlen(value3)), "put3");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key4, strlen(key4), value4, strlen(value4)), "put4");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key5, strlen(key5), value5, strlen(value5)), "put5");
    rchkAssertEqualsInt(0, rchkKVStorePut(store, key6, strlen(key6), value6, strlen(value6)), "put6");

    RchkKVStoreScanner* scanner = rchkKVStoreScanNew(store);

    rchkAssertNotNull(scanner, "scanner");

    RchkKVKeyValue current;
    while (!rchkKVStoreScanIsDone(scanner)) {
        rchkKVStoreScanGet(scanner, &current);

        RchkKVValue* value = rchkKVStoreGet(store, current.key, current.keySize);

        rchkAssertNotNull(value, "value");
        rchkAssertEqualsInt(current.valueSize, value->size, "value size");
        rchkAssertEqualsContent(current.value, value->value, current.valueSize, "value");

        rchkKVStoreScanMove(scanner);
    }
    rchkKVStoreScanFree(scanner);

    rchkKVStoreFree(store);

    printf("Test #10 passed\n");
}

int contains(char** list, int size, char* value) {
    for (int i = 0; i < size; i++) {
        if (strcmp(list[i], value) == 0) {
            return 1;
        }
    }

    return 0;
}

void testDeletion(char** all, int allSize, char** delete, int deleteSize) {
    RchkKVStore* store = rchkKVStoreNew2(testHash);

    rchkAssertNotNull(store, "store");

    for (int i=0; i<allSize; i++) {
        char* key = all[i];
        char* value = "xxx";
        int keySize = strlen(key);
        int valueSize = strlen(value);

        rchkAssertEqualsInt(0, rchkKVStorePut(store, key, keySize, value, valueSize), "put");
    }

    // Scanner init
    RchkKVStoreScanner* scanner = rchkKVStoreScanNew(store);

    rchkAssertNotNull(scanner, "scanner");

    RchkKVKeyValue current;
    while (!rchkKVStoreScanIsDone(scanner)) {
        rchkKVStoreScanGet(scanner, &current);

        if (contains(delete, deleteSize, current.key)) {
            rchkKVStoreScanDelete(scanner, NULL);
        } else {
            rchkKVStoreScanMove(scanner);
        }
    }
    rchkKVStoreScanFree(scanner);

    for (int i=0; i<allSize; i++) {
        char* key = all[i];

        if (contains(delete, deleteSize, key)) {
            rchkAssertNull(rchkKVStoreGet(store, key, strlen(key)), "not exists check");
        } else {
            rchkAssertNotNull(rchkKVStoreGet(store, key, strlen(key)), "exists check");
        }
    }

    rchkKVStoreFree(store);
}

void test11() {
    rchkTestSetName("Test #11");

    char* all[] = { "k1" };
    char* delete[] = { "k1" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #11 passed\n");
}

void test12() {
    rchkTestSetName("Test #12");

    char* all[] = { "k1", "k2", "k3", "k4", "k5", "k6"};
    char* delete[] = { "k1", "k2", "k3" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #12 passed\n");
}

void test13() {
    rchkTestSetName("Test #13");

    char* all[] = { "k1", "k2", "k3", "k4", "k5", "k6"};
    char* delete[] = { "k4", "k1" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #13 passed\n");
}

void test14() {
    rchkTestSetName("Test #14");

    char* all[] = { "k1", "k2", "k3", "k4", "k5", "k6"};
    char* delete[] = { "k5", "k2" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #14 passed\n");
}

void test15() {
    rchkTestSetName("Test #15");

    char* all[] = { "k1", "k2", "k3", "k4", "k5", "k6"};
    char* delete[] = { "k6", "k3" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #15 passed\n");
}

void test16() {
    rchkTestSetName("Test #16");

    char* all[] = { "k1", "k2", "k3", "k4", "k5", "k6"};
    char* delete[] = { "k4", "k1" };
    int allSize = sizeof(all) / sizeof(all[0]);
    int deleteSize = sizeof(delete) / sizeof(delete[0]);

    testDeletion(all, allSize, delete, deleteSize);

    printf("Test #16 passed\n");
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
    test9();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    return 0;
}

