#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"
#include "archke_time.h"

#define ARCHKE_SIMPLE_STRING_PREFIX "+"
#define ARCHKE_BINARY_STRING_PREFIX "$"
#define ARCHKE_DELIMETER "\r\n"
#define ARCHKE_NULL "_\r\n"
#define ARCHKE_OK "+OK\r\n"
#define ARCHKE_TRUE "#t\r\n"
#define ARCHKE_FALSE "#f\r\n"

// TODO: kvstore is not supposed to be here
RchkKVStore* kvstore; // stores data
RchkKVStore* expire; // stores when keys are supposed to expire
RchkKVStore* commands;

void initKvstore() {
    kvstore = rchkKVStoreNew();
    if (kvstore == NULL) {
        rchkExitFailure("Db keystore creation failed");
    }

    expire = rchkKVStoreNew();
    if (expire == NULL) {
        rchkExitFailure("Db keystore expire creation failed");
    }
}

void initCommands() {
    commands = rchkKVStoreNew();
    if (commands == NULL) {
        rchkExitFailure("Commands table creation failed");
    }

    rchkKVStorePut(commands, "ECHO", strlen("ECHO"), echoCommand, -1);
    rchkKVStorePut(commands, "SET",  strlen("SET"),  setCommand, -1);
    rchkKVStorePut(commands, "GET",  strlen("GET"),  getCommand, -1);
    rchkKVStorePut(commands, "DEL",  strlen("DEL"),  delCommand, -1);
    rchkKVStorePut(commands, "EXISTS",  strlen("EXISTS"),  existsCommand, -1);
    rchkKVStorePut(commands, "SHUTDOWN",  strlen("SHUTDOWN"),  shutdownCommand, -1);
}

RchkKVStore* getCommands() { return commands; }

/*
    Helper function. Used when key deletion happens to free memory from key and value
*/
void rchkDelFreeKeyValue(char* key, int keySize, void* value, int valueSize) {
    rchkFreeDuplicate(key, keySize);
    rchkFreeDuplicate(value, valueSize);
}

/*
    Helper function. Used when key deletion happens to free memory expiration date
*/
void rchkDelExpireValue(char* key, int keySize, void* value, int valueSize) {
    rchkFreeDuplicate(key, keySize);
    rchkFreeDuplicate(value, valueSize);
}

/*
    ECHO <msg>
    Response (simple string): +<msg>\r\n
*/
void echoCommand(RchkClient* client) {
    RchkArrayElement* msg = &client->commandElements[1];
    rchkAppendToReply(client, ARCHKE_SIMPLE_STRING_PREFIX, strlen(ARCHKE_SIMPLE_STRING_PREFIX));
    rchkAppendToReply(client, msg->bytes, msg->size);
    rchkAppendToReply(client, ARCHKE_DELIMETER, strlen(ARCHKE_DELIMETER));
}

uint64_t rchkStringToULInt(const char* string, const int size) {
    char timeoutDup[size + 1];
    strncpy(timeoutDup, string, size);
    timeoutDup[size] = '\0';

    return strtoul(timeoutDup, NULL, 10);
}

/*
    SET <key> <val>
    Response (simple string): +OK\r\n
*/
void setCommand(RchkClient* client) {
    // 1.
    RchkArrayElement* key = &client->commandElements[1];
    RchkArrayElement* value = &client->commandElements[2];

    char* keyDup = rchkDuplicate(key->bytes, key->size);
    char* valueDup = rchkDuplicate(value->bytes, value->size);

    if (rchkKVStorePut(kvstore, keyDup, key->size, valueDup, value->size) < 0) {
        // TODO: write better error error handling
        rchkExitFailure("'set' operation failed");
    }

    // set expire
    if (client->commandElementsCount > 4) {
        uint64_t* timeout = malloc(sizeof(uint64_t));
        if (timeout == NULL) {
            rchkExitFailure("PANIC: memory allocation failed");
            return;
        }

        const RchkArrayElement* timeoutElement = &client->commandElements[4];

        *timeout = rchkStringToULInt(timeoutElement->bytes, timeoutElement->size);

        const uint64_t now = getMonotonicUs();

        *timeout = now + *timeout * 1000;

        char* keyDup2 = rchkDuplicate(key->bytes, key->size);
        if (rchkKVStorePut(expire, keyDup2, key->size, timeout, -1) < 0) {
            // TODO: write better error error handling
            rchkExitFailure("'set' operation failed");
        }
    }

    // 2.
    rchkAppendToReply(client, ARCHKE_OK, strlen(ARCHKE_OK));
}

/*
    GET <key>
    Response (binary string) : $<value_length>\r\n<value>
    Response not found (null) : _\r\n
*/
void getCommand(RchkClient* client) {
    RchkArrayElement* key = &client->commandElements[1];

    RchkKVValue* value = rchkKVStoreGet(kvstore, key->bytes, key->size);
    RchkKVValue* timeout = rchkKVStoreGet(expire, key->bytes, key->size);

    if (value != NULL) {
        // check expire
        if (timeout != NULL) {
            uint64_t now = getMonotonicUs();
            uint64_t* when = timeout->value;
            if (now > *when) {
                rchkKVStoreDelete2(kvstore, key->bytes, key->size, rchkDelFreeKeyValue);
                rchkKVStoreDelete2(expire, key->bytes, key->size, rchkDelExpireValue);

                rchkAppendToReply(client, ARCHKE_NULL, strlen(ARCHKE_NULL));
                return;
            }
        }

        rchkAppendToReply(client, ARCHKE_BINARY_STRING_PREFIX, strlen(ARCHKE_BINARY_STRING_PREFIX));
        rchkAppendIntegerToReply(client, value->size);
        rchkAppendToReply(client, ARCHKE_DELIMETER, strlen(ARCHKE_DELIMETER));
        rchkAppendToReply(client, value->value, value->size);
    } else {
        rchkAppendToReply(client, ARCHKE_NULL, strlen(ARCHKE_NULL));
    }
}

/*
    EXISTS <key>
    Response (boolean) : #<t|f>\r\n
    t - key/value pair exists
    f - key/value pair does not exist
*/
void existsCommand(RchkClient* client) {
    RchkArrayElement* key = &client->commandElements[1];

    RchkKVValue* value = rchkKVStoreGet(kvstore, key->bytes, key->size);

    if (value != NULL) {
        rchkAppendToReply(client, ARCHKE_TRUE, strlen(ARCHKE_TRUE));
    } else {
        rchkAppendToReply(client, ARCHKE_FALSE, strlen(ARCHKE_FALSE));
    }
}

/*
    DELETE <key>
    Response (Integer reply), the number of keys that were removed - :<integer>\r\n
*/
void delCommand(RchkClient* client) {
    // 1.
    RchkArrayElement* key = &client->commandElements[1];

    int deleted = rchkKVStoreDelete2(kvstore, key->bytes, key->size, rchkDelFreeKeyValue);
    rchkKVStoreDelete2(expire, key->bytes, key->size, rchkDelExpireValue);

    // 2.
    if (deleted < 1) {
        rchkAppendToReply(client, ":0\r\n", strlen(":0\r\n"));    
    } else {
        rchkAppendToReply(client, ":1\r\n", strlen(":1\r\n"));
    } 
}

void shutdownCommand(RchkClient* client) {
    exit(0);
}
