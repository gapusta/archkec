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
    char* value;
    int valueSize;
    struct RchkBucketNode* next;
} RchkBucketNode;

static RchkBucketNode* buckets[ARCHKE_BUCKETS];

static uint64_t _rchkHash(const char* target, int targetSize) {
    uint64_t hash = FNV_OFFSET;
    for (int i=0; i<targetSize; i++) {
        hash ^= (uint64_t)(unsigned char)(target[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

int rchkKVStoreInit() {
    for (int i=0; i<ARCHKE_BUCKETS; i++) {
        buckets[i] = NULL;
    }

    return 0;
}

RchkBucketNode* _rchkKVStoreSearch(uint64_t bucket, char* key, int keySize) {
    RchkBucketNode* next = buckets[bucket];

    while(next != NULL) {
        if (next->keySize != keySize) {
            next = next->next;
            continue;
        }

        for (int i=0; i<keySize; i++) {
            if (next->key[i] != key[i]) {
                next = next->next;
                continue;
            }
        }

        return next; 
    }

    return NULL;
}

int rchkKVStorePut(char* key, int keySize, char* value, int valueSize) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(index, key, keySize);
    if (node != NULL) {
        node->value = value;
        node->valueSize = valueSize;
        return 0;
    }

    RchkBucketNode* new = malloc(sizeof(RchkBucketNode));
    if (new == NULL) {
        return -1;
    }
    
    new->key = key;
    new->keySize = keySize;
    new->value = value;
    new->valueSize = valueSize;
    new->next = buckets[index];

    buckets[index] = new;

    return 0;    
}

int rchkKVStoreGet(char* key, int keySize, char** value, int* valueSize) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(index, key, keySize);
    if (node != NULL) {
        (*value) = node->value;
        (*valueSize) = node->valueSize;
        
        return 0;
    }

    return ARCHKE_KVSTORE_NOT_FOUND;
}

