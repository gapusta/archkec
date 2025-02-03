#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "archke_kvstore.h"

// hash function link - https://benhoyt.com/writings/hash-table-in-c/

#define ARCHKE_BUCKETS 1024
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct RchkBucketNode {
    char* key;
    int keySize;
    RchkKVValue* value;
    struct RchkBucketNode* next;
} RchkBucketNode;

struct RchkKVStore {
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

RchkKVStore* rchkKVStoreNew() {
    RchkKVStore* new = malloc(sizeof(RchkKVStore));
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

// TODO: SEGFAULT happens here
RchkBucketNode* _rchkKVStoreSearch(RchkKVStore* store, uint64_t bucketIndex, char* key, int keySize) {
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

int rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        RchkKVValue* found = node->value;
        found->value = value;
        found->size = valueSize;
        return 0;
    }

    RchkKVValue* valueHolder = (RchkKVValue*) malloc(sizeof(RchkKVValue));
    if (valueHolder == NULL) {
        return -1;
    }
    valueHolder->value = value;
    valueHolder->size = valueSize;

    RchkBucketNode* new = (RchkBucketNode*) malloc(sizeof(RchkBucketNode));
    if (new == NULL) {
        free(valueHolder);
        return -1;
    }
    new->value = valueHolder;
    new->key = key;
    new->keySize = keySize;
    new->next = store->buckets[index];

    store->buckets[index] = new;

    return 0;    
}

RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize) {
    uint64_t index = _rchkHash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        return node->value;
    }

    return NULL;
}

void  rchkKVStoreFree(RchkKVStore* store) {
    RchkBucketNode* current;
    RchkBucketNode* next;

    for (int i=0; i<ARCHKE_BUCKETS; i++) {
        current = store->buckets[i];

        while (current != NULL)
        {
            next = current->next;
            free(current->value);
            free(current);
            current = next;
        }
    }

    free(store->buckets);
    free(store);
}

