#ifndef ARCHKE_KVSTORE_HASH
#define ARCHKE_KVSTORE_HASH

#include <stdint.h>

typedef struct RchkKVStore RchkKVStore;
typedef struct RchkKVStoreScanner RchkKVStoreScanner;

typedef struct RchkKVValue {
    void* value;
    int size;
} RchkKVValue;

typedef struct RchkKVKeyValue {
    char* key;
    int keySize;
    void* value;
    int valueSize;
} RchkKVKeyValue;

typedef uint64_t rchkKVStoreHash(const char* target, int targetSize);
typedef void rchkKVStoreFreeKeyValue(char* key, int keySize, void* value, int valueSize);
typedef void rchkKVStoreScanCallback(char* key, int keySize, void* value, int valueSize, void* callbackData);

RchkKVStore* rchkKVStoreNew();
RchkKVStore* rchkKVStoreNew2(rchkKVStoreHash* hash, int initialSize);
int          rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize);
RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize);
int          rchkKVStoreDelete(RchkKVStore* store, char* key, int keySize);
int          rchkKVStoreDelete2(RchkKVStore* store, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue);
void         rchkKVStoreFree(RchkKVStore* store);
void         rchkKVStoreFree2(RchkKVStore* store, rchkKVStoreFreeKeyValue* freeKeyValue);

/*
 Incremental rehashing API
 */
void rchkKVStoreRehashActivateIfNeeded(RchkKVStore* store);
int  rchkKVStoreRehashActive(RchkKVStore* store);
void rchkKVStoreRehashStep(RchkKVStore* store);

/*
 Used to iterate over the elements of a kvstore.

 Iterating works the following way:

 1) Initially you call the function using a cursor (v) value of 0.
 2) The function performs one step of the iteration, and returns the
     new cursor value you must use in the next call.
 3) When the returned cursor is 0, the iteration is complete.
 */
uint64_t rchkKVStoreScan(RchkKVStore* store, uint64_t cursor, rchkKVStoreScanCallback* callback, void* callbackData);

#endif
