#ifndef ARCHKE_KVSTORE_HASH
#define ARCHKE_KVSTORE_HASH

typedef struct RchkKVStore RchkKVStore;
typedef struct RchkKVValue {
    void* value;
    int size;
} RchkKVValue;

RchkKVStore* rchkKVStoreNew();
int          rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize);
RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize);
void         rchkKVStoreFree(RchkKVStore* store);

#endif

