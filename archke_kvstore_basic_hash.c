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
    RchkBucketNode** new; // new table during incremental rehashing
    rchkKVStoreHash* hash;
    u_int64_t used; // total number of elements
    u_int64_t size; // current table size
    // mask = size - 1. for example is size = 16 then mask is 0..01111 (16 - 1 = 15 in binary)
    u_int64_t mask;
    u_int64_t newSize; // new table size
    // newMask = newSize - 1. for example is size = 64 then mask is 0..0111111 (64 - 1 = 63 in binary)
    u_int64_t newMask;
    u_int64_t rehashIdx; // next bucket to move during a rehash step
};

struct RchkKVStoreScanner {
    RchkBucketNode** buckets;
    int index; /* current bucket index */
    RchkBucketNode* prev; /* previous element required for effective removal */
    RchkBucketNode* current;
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
    return rchkKVStoreNew2(NULL, ARCHKE_BUCKETS_INIT_SIZE);
}

RchkKVStore* rchkKVStoreNew2(rchkKVStoreHash* hash, int initialSize) {
    RchkKVStore* new = malloc(sizeof(RchkKVStore));
    if (new == NULL) {
        return NULL;
    }

    // allocates array and inits all values to NULL
    new->buckets = calloc(initialSize, sizeof(RchkBucketNode*));
    if (new->buckets == NULL) {
        free(new);
        return NULL;
    }

    new->hash = hash != NULL ? hash : _rchkHash;
    new->size = initialSize;
    new->mask = initialSize - 1;
    new->used = 0;

    new->new = NULL;
    new->newSize = 0;
    new->newMask = 0;
    new->rehashIdx = 0;

    return new;
}

RchkBucketNode* _rchkKVStoreSearchInBuckets(RchkBucketNode** buckets, u_int64_t mask, rchkKVStoreHash* hash, char* key, int keySize) {
    uint64_t index = hash(key, keySize) & mask;
    RchkBucketNode* node = buckets[index];
    
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

RchkBucketNode* _rchkKVStoreSearch(RchkKVStore* store, char* key, int keySize) {
    // 1. Search in the current table
    RchkBucketNode* node = _rchkKVStoreSearchInBuckets(store->buckets, store->mask, store->hash, key, keySize);
    if (node != NULL) {
        return node;
    }

    // 2. Search in new table if rehashing is in progress
    if (rchkKVStoreRehashActive(store)) {
        node = _rchkKVStoreSearchInBuckets(store->new, store->newMask, store->hash, key, keySize);
        if (node != NULL) {
            return node;
        }
    }

    // 3. return NULL if both tables are empty
    return NULL;
}

int rchkKVStorePut(RchkKVStore* store, char* key, int keySize, void* value, int valueSize) {
    // 1. Update existing key-value pair if exists
    RchkBucketNode* node = _rchkKVStoreSearch(store, key, keySize);
    if (node != NULL) {
        RchkKVValue* found = node->value;
        found->value = value;
        found->size = valueSize;
        return 0;
    }

    // 2. Add new key-value pair if it does not exist
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

    store->used++;

    if (rchkKVStoreRehashActive(store)) {
        uint64_t index = store->hash(key, keySize) & store->newMask;
        new->next = store->new[index];
        store->new[index] = new;
    } else {
        uint64_t index = store->hash(key, keySize) & store->mask;
        new->next = store->buckets[index];
        store->buckets[index] = new;
        rchkKVStoreRehashActivateIfNeeded(store);
    }

    return 0;
}

RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize) {
    RchkBucketNode* node = _rchkKVStoreSearch(store, key, keySize);
    if (node != NULL) {
        return node->value;
    }

    return NULL;
}

static int _rchkKVStoreDelete(RchkBucketNode** buckets, rchkKVStoreHash* hash, u_int64_t mask, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue) {
    uint64_t index = hash(key, keySize) & mask;

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

int rchkKVStoreDelete2(RchkKVStore* store, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue) {
    // 1. Try to delete from current table
    int result = _rchkKVStoreDelete(store->buckets, store->hash, store->mask, key, keySize, freeKeyValue);
    if (result > 0) {
        store->used--;
        rchkKVStoreRehashActivateIfNeeded(store);
        return result;
    }

    // 2. Try to delete from new table if rehashing is in progress
    if (rchkKVStoreRehashActive(store)) {
        result = _rchkKVStoreDelete(store->new, store->hash, store->newMask, key, keySize, freeKeyValue);
        if (result > 0) {
            store->used--;
            return result;
        }
    }

    return 0;
}

int rchkKVStoreDelete(RchkKVStore* store, char* key, int keySize) {
    return rchkKVStoreDelete2(store, key, keySize, NULL);
}

void  rchkKVStoreFree(RchkKVStore* store) {
    rchkKVStoreFree2(store, NULL);
}

void _rchkKVStoreFreeBuckets(RchkBucketNode** buckets, u_int64_t size, rchkKVStoreFreeKeyValue* freeKeyValue) {
    for (int i=0; i<size; i++) {
        RchkBucketNode *current = buckets[i];

        while (current != NULL) {
            RchkBucketNode *next = current->next;
            if (freeKeyValue != NULL) {
                freeKeyValue(current->key, current->keySize, current->value->value, current->value->size);
            }
            free(current->value);
            free(current);
            current = next;
        }
    }
}

void rchkKVStoreFree2(RchkKVStore* store, rchkKVStoreFreeKeyValue* freeKeyValue) {
    // 1. Free primary table
    _rchkKVStoreFreeBuckets(store->buckets, store->size, freeKeyValue);
    free(store->buckets);

    // 2. Free secondary table if needed
    if (rchkKVStoreRehashActive(store)) {
        _rchkKVStoreFreeBuckets(store->new, store->newSize, freeKeyValue);
        free(store->new);
    }

    free(store);
}

void rchkKVStoreRehashActivateIfNeeded(RchkKVStore* store) {
    if (rchkKVStoreRehashActive(store)) {
        return;
    }

    // Resize up if needed
    if (store->used >= store->size) {
        u_int64_t newSize = store->size * 2;
        store->rehashIdx = 0;
        store->newSize = newSize;
        store->newMask = newSize - 1;
        store->new = calloc(newSize, sizeof(RchkBucketNode*));
        if (store->new == NULL) {
            rchkExitFailure("Fatal: OOM error during KV store resize up");
        }
    }
}

int rchkKVStoreRehashActive(RchkKVStore* store) {
    return store->new != NULL;
}

void rchkKVStoreRehashStep(RchkKVStore* store) {
    if (!rchkKVStoreRehashActive(store)) {
        return;
    }

    // 1. remove bucket from primary table
    RchkBucketNode* current = store->buckets[store->rehashIdx];
    store->buckets[store->rehashIdx] = NULL;

    // 2. scatter bucket elements across new/secondary table
    while (current != NULL) {
        RchkBucketNode* next = current->next;

        uint64_t index = store->hash(current->key, current->keySize) % store->newSize;
        current->next = store->new[index];
        store->new[index] = current;

        current = next;
    }
    store->rehashIdx++;

    // 3. complete the rehashing if needed
    if (store->rehashIdx >= store->size) {
        free(store->buckets);
        store->buckets = store->new;
        store->size = store->newSize;
        store->mask = store->newMask;
        store->new = NULL;
        store->newSize = 0;
        store->newMask = 0;
        store->rehashIdx = 0;
    }
}

/* Scanner implementation */

static u_int64_t rchkKVStoreScanGetNonEmptyBucket(RchkKVStore* store, int start) {
    for (int bucket=start; bucket<store->size; bucket++) {
        if (store->buckets[bucket] != NULL) {
            return bucket;
        }
    }

    return store->size;
}

u_int64_t rchkKVStoreScan(RchkKVStore* store, int cursor, rchkKVStoreScanCallback* callback, void* callbackData) {
    if (cursor < 0 || cursor > store->size) {
        // invalid cursor value
        return -1;
    }

    u_int64_t bucket = rchkKVStoreScanGetNonEmptyBucket(store, cursor);
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
