#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "archke_error.h"
#include "archke_kvstore.h"

// hash function link - https://benhoyt.com/writings/hash-table-in-c/

#define ARCHKE_BUCKETS_INIT_SIZE 4
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct RchkBucketNode {
    char* key;
    int keySize;
    RchkKVValue* value;
    struct RchkBucketNode* next;
} RchkBucketNode;

struct RchkKVStore {
    RchkBucketNode** buckets; // primary table
    rchkKVStoreHash* hash;
    int size;
    int used;

    RchkBucketNode** new; // new table during incremental rehashing
    int newSize; // new table size
    int ridx; // next bucket to move during rehashing step
};

struct RchkKVStoreScanner {
    RchkBucketNode** buckets;
    int index; /* current bucket index */
    RchkBucketNode* prev; /* previous element required for effective removal */
    RchkBucketNode* current;
};

static int _rchkKVStoreExpandNeeded(RchkKVStore* store);

static void _rchkKVStoreStartExpansionIfNeeded(RchkKVStore* store);

static uint64_t _rchkHash(const char* target, int targetSize) {
    uint64_t hash = FNV_OFFSET;
    for (int i=0; i<targetSize; i++) {
        hash ^= (uint64_t)(unsigned char)(target[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

RchkKVStore* rchkKVStoreNew() {
    return rchkKVStoreNew2(NULL, ARCHKE_BUCKETS_INIT_SIZE);
}

RchkKVStore* rchkKVStoreNew2(rchkKVStoreHash* hash, int initialSize) {
    RchkKVStore* new = malloc(sizeof(RchkKVStore));
    if (new == NULL) {
        return NULL;
    }

    new->buckets = malloc(initialSize * sizeof(RchkBucketNode*));
    if (new->buckets == NULL) {
        free(new);
        return NULL;
    }

    for (int i=0; i<initialSize; i++) {
        new->buckets[i] = NULL;
    }

    new->hash = hash != NULL ? hash : _rchkHash;
    new->size = initialSize;
    new->used = 0;

    new->new = NULL;
    new->newSize = 0;
    new->ridx = 0;

    return new;
}

RchkBucketNode* _rchkKVStoreSearch(RchkBucketNode** buckets, uint64_t bucketIndex, char* key, int keySize) {
    RchkBucketNode* node = buckets[bucketIndex];
    
next:
    while(node != NULL) {
        if (node->keySize != keySize) {
            node = node->next;
            continue;
        }

        for (int i=0; i<keySize; i++) {
            if (node->key[i] != key[i]) {
                node = node->next;
                goto next;
            }
        }

        return node; 
    }

    return NULL;
}

int rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize) {
    RchkBucketNode** buckets = store->buckets;
    int size = store->size;
    uint64_t index = store->hash(key, keySize) % size;

    RchkBucketNode* node = _rchkKVStoreSearch(buckets, index, key, keySize);
    if (node != NULL) {
        RchkKVValue* found = node->value;
        found->value = value;
        found->size = valueSize;
        return 0;
    }

    if (rchkKVStoreRehashActive(store)) {
        buckets = store->new;
        size = store->newSize;
        index = store->hash(key, keySize) % size;

        node = _rchkKVStoreSearch(buckets, index, key, keySize);
        if (node != NULL) {
            RchkKVValue* found = node->value;
            found->value = value;
            found->size = valueSize;
            return 0;
        }
    }

    RchkKVValue* val = (RchkKVValue*) malloc(sizeof(RchkKVValue));
    if (val == NULL) {
        return -1;
    }
    val->value = value;
    val->size = valueSize;

    RchkBucketNode* new = (RchkBucketNode*) malloc(sizeof(RchkBucketNode));
    if (new == NULL) {
        free(val);
        return -1;
    }
    new->value = val;
    new->key = key;
    new->keySize = keySize;
    new->next = buckets[index];

    buckets[index] = new;
    store->used++;

    _rchkKVStoreStartExpansionIfNeeded(store);

    return 0;    
}

RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize) {
    uint64_t index = store->hash(key, keySize) % store->size;
    RchkBucketNode* node = _rchkKVStoreSearch(store->buckets, index, key, keySize);
    if (node != NULL) {
        return node->value;
    }

    if (rchkKVStoreRehashActive(store)) {
        index = store->hash(key, keySize) % store->newSize;
        node = _rchkKVStoreSearch(store->new, index, key, keySize);
        if (node != NULL) {
            return node->value;
        }
    }

    return NULL;
}

static int _rchkKVStoreDelete(RchkBucketNode** buckets, rchkKVStoreHash* hash, int size, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue) {
    uint64_t index = hash(key, keySize) % size;

    RchkBucketNode* first = buckets[index];
    RchkBucketNode* current = first;
    RchkBucketNode* prev = NULL;

    next:
        while(current != NULL) {
            if (current->keySize != keySize) {
                prev = current;
                current = current->next;
                continue;
            }

            for (int i=0; i<keySize; i++) {
                if (current->key[i] != key[i]) {
                    prev = current;
                    current = current->next;
                    goto next;
                }
            }

            if (current == first) {
                buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }

            if (freeKeyValue != NULL) {
                freeKeyValue(current->key, current->keySize, current->value->value, current->value->size);
            }
            free(current->value);
            free(current);

            return 1; // one element has been found and deleted
        }

    return 0; // zero elements has been found and deleted
}

int rchkKVStoreDelete(RchkKVStore* store, char* key, int keySize) {
    return rchkKVStoreDelete2(store, key, keySize, NULL);
}

int rchkKVStoreDelete2(RchkKVStore* store, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue) {
    int result = _rchkKVStoreDelete(store->buckets, store->hash, store->size, key, keySize, freeKeyValue);
    if (result > 0) {
        return result;
    }
    if (!rchkKVStoreRehashActive(store)) {
        return 0;
    }
    return _rchkKVStoreDelete(store->new, store->hash, store->newSize, key, keySize, freeKeyValue);
}

void  rchkKVStoreFree(RchkKVStore* store) {
    rchkKVStoreFree2(store, NULL);
}

void rchkKVStoreFree2(RchkKVStore* store, rchkKVStoreFreeKeyValue* freeKeyValue) {
    RchkBucketNode* current;
    RchkBucketNode* next;

    for (int i=0; i<store->size; i++) {
        current = store->buckets[i];

        while (current != NULL) {
            next = current->next;
            if (freeKeyValue != NULL) {
                freeKeyValue(current->key, current->keySize, current->value->value, current->value->size);
            }
            free(current->value);
            free(current);
            current = next;
        }
    }
    free(store->buckets);

    if (rchkKVStoreRehashActive(store)) {
        for (int i=0; i<store->newSize; i++) {
            current = store->new[i];

            while (current != NULL) {
                next = current->next;
                if (freeKeyValue != NULL) {
                    freeKeyValue(current->key, current->keySize, current->value->value, current->value->size);
                }
                free(current->value);
                free(current);
                current = next;
            }
        }
    }
    free(store->new);

    free(store);
}

void _rchkKVStoreStartExpansionIfNeeded(RchkKVStore* store) {
    int expansionNeeded = _rchkKVStoreExpandNeeded(store);
    int rehashActive = rchkKVStoreRehashActive(store);
    int newSize = store->size * 2;

    if (expansionNeeded && !rehashActive) {
        store->ridx = 0;
        store->newSize = newSize;
        store->new = malloc(newSize * sizeof(RchkBucketNode*));
        if (store->new == NULL) {
            rchkExitFailure("Fatal: OOM error during KV store resize");
        }
        for (int i=0; i<newSize; i++) {
            store->new[i] = NULL;
        }
    }
}

int _rchkKVStoreExpandNeeded(RchkKVStore* store) {
    return store->used >= store->size;
}

int rchkKVStoreRehashActive(RchkKVStore* store) {
    return store->new != NULL;
}

void rchkKVStoreRehashStep(RchkKVStore* store) {
    if (!rchkKVStoreRehashActive(store)) {
        return;
    }

    // 1. remove bucket from old table
    RchkBucketNode* current = store->buckets[store->ridx];
    store->buckets[store->ridx] = NULL;

    // 2. scatter bucket elements across new table
    while (current != NULL) {
        RchkBucketNode* next = current->next;

        uint64_t index = store->hash(current->key, current->keySize) % store->newSize;
        current->next = store->new[index];
        store->new[index] = current;

        current = next;
    }
    store->ridx++;

    // 3. check if rehashing is done
    if (store->ridx >= store->size) {
        free(store->buckets);
        store->buckets = store->new;
        store->size = store->newSize;
        store->new = NULL;
        store->ridx = 0;
    }
}

/* Scanner implementation */

static int rchkKVStoreScanGetNonEmptyBucket(RchkKVStore* store, int start) {
    for (int bucket=start; bucket<store->size; bucket++) {
        if (store->buckets[bucket] != NULL) {
            return bucket;
        }
    }

    return store->size;
}

int rchkKVStoreScan(RchkKVStore* store, int cursor, rchkKVStoreScanCallback* callback, void* callbackData) {
    if (cursor < 0 || cursor > store->size) {
        // invalid cursor value
        return -1;
    }

    int bucket = rchkKVStoreScanGetNonEmptyBucket(store, cursor);
    if (bucket == store->size) {
        return 0;
    }

    RchkBucketNode* next;
    RchkBucketNode* current = store->buckets[bucket];;
    do {
        next = current->next;
        callback(current->key, current->keySize, current->value->value, current->value->size, callbackData);
        current = next;
    } while (current != NULL);

    return bucket + 1;
}
