#ifndef ARCHKE_KVSTORE_HASH
#define ARCHKE_KVSTORE_HASH

#define ARCHKE_KVSTORE_NOT_FOUND 1

int rchkKVStoreInit();
int rchkKVStorePut(char* key, int keySize, char* value, int valueSize);
int rchkKVStoreGet(char* key, int keySize, char** value, int* valueSize);

#endif

