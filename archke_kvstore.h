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
    void* key;
    int keySize;
    void* value;
    int valueSize;
} RchkKVKeyValue;

typedef uint64_t rchkKVStoreHash(const char* target, int targetSize);
typedef void rchkKVStoreFreeKeyValue(char* key, int keySize, void* value, int valueSize);

RchkKVStore* rchkKVStoreNew();
RchkKVStore* rchkKVStoreNew2(rchkKVStoreHash* hash);
int          rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize);
RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize);
int          rchkKVStoreDelete(RchkKVStore* store, char* key, int keySize);
int          rchkKVStoreDelete2(RchkKVStore* store, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue);
void         rchkKVStoreFree(RchkKVStore* store);
void         rchkKVStoreFree2(RchkKVStore* store, rchkKVStoreFreeKeyValue* freeKeyValue);

/* Scanner API */

RchkKVStoreScanner* rchkKVStoreScanNew(RchkKVStore* store);
void rchkKVStoreScanFree(RchkKVStoreScanner* scanner);
int  rchkKVStoreScanIsDone(RchkKVStoreScanner* scanner);
void rchkKVStoreScanGet(RchkKVStoreScanner* scanner, RchkKVKeyValue* holder); /* Get current element */
void rchkKVStoreScanMove(RchkKVStoreScanner* scanner); /* Move to next element */
void rchkKVStoreScanDelete(RchkKVStoreScanner* scanner, rchkKVStoreFreeKeyValue* freeKeyValue); /* Deletes current element */

#endif

