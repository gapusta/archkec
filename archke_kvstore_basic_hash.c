#include <stdlib.h>
#include <stdint.h>
#include "archke_kvstore.h"

// hash function link - https://benhoyt.com/writings/hash-table-in-c/

#define ARCHKE_BUCKETS 1024
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct RchkBucketNode {
    char* key;
    int keySize;
    void* value;
    struct RchkBucketNode* next;
} RchkBucketNode;

struct KVStore {
    RchkBucketNode** buckets;
};

static uint64_t _rchkHash(const char* target, int targetSize) {
    uint64_t hash = FNV_OFFSET;
    for (int i=0; i<targetSize; i++) {
        hash ^= (uint64_t)(unsigned char)(target[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

KVStore* rchkKVStoreNew() {
    KVStore* new = malloc(sizeof(KVStore));
    if (new == NULL) {
        return NULL;
    }

    new->buckets = malloc(ARCHKE_BUCKETS * sizeof(RchkBucketNode*));
    if (new->buckets == NULL) {
        free(new);
        return NULL;
    }

    for (int i=0; i<ARCHKE_BUCKETS; i++) {
        new->buckets[i] = NULL;
    }

    return new;
}

RchkBucketNode* _rchkKVStoreSearch(KVStore* store, uint64_t bucketIndex, char* key, int keySize) {
    RchkBucketNode* node = store->buckets[bucketIndex];

    while(node != NULL) {
        if (node->keySize != keySize) {
            node = node->next;
            continue;
        }

        for (int i=0; i<keySize; i++) {
            if (node->key[i] != key[i]) {
                node = node->next;
                continue;
            }
        }

        return node; 
    }

    return NULL;
}

int rchkKVStorePut(KVStore* store, char* key, int keySize, void* value) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        node->value = value;
        return 0;
    }

    RchkBucketNode* new = malloc(sizeof(RchkBucketNode));
    if (new == NULL) {
        return -1;
    }
    
    new->key = key;
    new->keySize = keySize;
    new->value = value;
    new->next = store->buckets[index];

    store->buckets[index] = new;

    return 0;    
}

void* rchkKVStoreGet(KVStore* store, char* key, int keySize) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        return node->value;
    }

    return NULL;
}

void  rchkKVStoreFree(KVStore* store) {
    RchkBucketNode* current;
    RchkBucketNode* next;

    for (int i=0; i<ARCHKE_BUCKETS; i++) {
        current = store->buckets[i];

        while (current != NULL)
        {
            next = current->next;
            free(current);
            current = next;
        }
    }

    free(store->buckets);
    free(store);
}

