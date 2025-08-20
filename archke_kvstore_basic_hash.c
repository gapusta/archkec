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
    rchkKVStoreHash* hash;
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
    return rchkKVStoreNew2(NULL);
}

RchkKVStore* rchkKVStoreNew2(rchkKVStoreHash* hash) {
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

    new->hash = hash != NULL ? hash : _rchkHash;

    return new;
}

RchkBucketNode* _rchkKVStoreSearch(RchkKVStore* store, uint64_t bucketIndex, char* key, int keySize) {
    RchkBucketNode* node = store->buckets[bucketIndex];
    
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
    uint64_t index = store->hash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        RchkKVValue* found = node->value;
        found->value = value;
        found->size = valueSize;
        return 0;
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
    new->next = store->buckets[index];

    store->buckets[index] = new;

    return 0;    
}

RchkKVValue* rchkKVStoreGet(RchkKVStore* store, char* key, int keySize) {
    uint64_t index = store->hash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* node = _rchkKVStoreSearch(store, index, key, keySize);
    if (node != NULL) {
        return node->value;
    }

    return NULL;
}

int rchkKVStoreDelete(RchkKVStore* store, char* key, int keySize) {
    return rchkKVStoreDelete2(store, key, keySize, NULL);
}

int rchkKVStoreDelete2(RchkKVStore* store, char* key, int keySize, rchkKVStoreFreeKeyValue* freeKeyValue) {
    uint64_t index = store->hash(key, keySize) % ARCHKE_BUCKETS;

    RchkBucketNode* first = store->buckets[index];
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
            store->buckets[index] = current->next;
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

void  rchkKVStoreFree(RchkKVStore* store) {
    rchkKVStoreFree2(store, NULL);
}

void rchkKVStoreFree2(RchkKVStore* store, rchkKVStoreFreeKeyValue* freeKeyValue) {
    RchkBucketNode* current;
    RchkBucketNode* next;

    for (int i=0; i<ARCHKE_BUCKETS; i++) {
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
    free(store);
}

/* Scanner API implementation */

static int rchkKVStoreScanGetNonEmptyBucket(RchkBucketNode** buckets, int start) {
    for (int bucket=start; bucket<ARCHKE_BUCKETS; bucket++) {
        if (buckets[bucket] != NULL) {
            return bucket;
        }
    }

    return ARCHKE_BUCKETS;
}

int rchkKVStoreScanIsDone(RchkKVStoreScanner* scanner) {
    return scanner->current == NULL;
}

RchkKVStoreScanner* rchkKVStoreScanNew(RchkKVStore* store) {
    RchkKVStoreScanner* scanner = malloc(sizeof(RchkKVStoreScanner));
    if (scanner == NULL) {
        return NULL;
    }

    scanner->buckets = store->buckets;
    scanner->index = rchkKVStoreScanGetNonEmptyBucket(store->buckets, 0);
    scanner->prev = NULL;
    if (scanner->index < ARCHKE_BUCKETS) {
        scanner->current = scanner->buckets[scanner->index];
    } else {
        scanner->current = NULL;
    }

    return scanner;
}

void rchkKVStoreScanFree(RchkKVStoreScanner* scanner) {
    free(scanner);
}

void rchkKVStoreScanGet(RchkKVStoreScanner* scanner, RchkKVKeyValue* holder) {
    RchkBucketNode* current = scanner->current;

    holder->key = current->key;
    holder->keySize = current->keySize;
    holder->value = current->value->value;
    holder->valueSize = current->value->size;
}

void rchkKVStoreScanMove(RchkKVStoreScanner* scanner) {
    if (rchkKVStoreScanIsDone(scanner)) { return; }

    if (scanner->current->next != NULL) {
        scanner->prev = scanner->current;
        scanner->current = scanner->current->next;
        return;
    }

    // move to next bucket
    scanner->index = rchkKVStoreScanGetNonEmptyBucket(scanner->buckets, scanner->index + 1);

    if (scanner->index >= ARCHKE_BUCKETS) {
        scanner->prev = scanner->current;
        scanner->current = NULL;
    } else {
        scanner->prev = NULL;
        scanner->current = scanner->buckets[scanner->index];
    }
}

// Deletes current element
void rchkKVStoreScanDelete(RchkKVStoreScanner* scanner, rchkKVStoreFreeKeyValue* freeKeyValue) {
    if (rchkKVStoreScanIsDone(scanner)) { return; }

    int index = scanner->index;
    RchkBucketNode* prev = scanner->prev;
    RchkBucketNode* current = scanner->current;

    // move to next node
    rchkKVStoreScanMove(scanner);

    // remove from scanner
    scanner->prev = prev;

    // remove from table
    if (prev == NULL) {
        scanner->buckets[index] = current->next;
    } else {
        prev->next = current->next;
    }

    // remove from existence
    if (freeKeyValue != NULL) {
        freeKeyValue(current->key, current->keySize, current->value->value, current->value->size);
    }
    free(current->value);
    free(current);
}
