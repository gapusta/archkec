#ifndef ARCHKE_KVSTORE_HASH
#define ARCHKE_KVSTORE_HASH

typedef struct KVStore KVStore;

KVStore* rchkKVStoreNew();
int   rchkKVStorePut(KVStore* store, char* key, int keySize, void* value);
void* rchkKVStoreGet(KVStore* store, char* key, int keySize);
void  rchkKVStoreFree(KVStore* store);

#endif

