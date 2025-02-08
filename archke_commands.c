#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"

#define ARCHKE_SIMPLE_STRING_PREFIX "+"
#define ARCHKE_BINARY_STRING_PREFIX "$"
#define ARCHKE_DELIMETER "\r\n"
#define ARCHKE_NULL "_\r\n"
#define ARCHKE_OK "+OK\r\n"

// TODO: kvstore is not supposed to be here
RchkKVStore* kvstore; // stores data
RchkKVStore* commands;

void initKvstore() {
    kvstore = rchkKVStoreNew();
    if (kvstore == NULL) {
        rchkExitFailure("Db keystore creation failed");
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
}

RchkKVStore* getCommands() { return commands; }

/*
    ECHO <msg>
    Response (simple string): +<msg>\r\n
*/
void echoCommand(RchkClient* client) {
    RchkArrayElement* msg = &client->in[1]; 
    rchkAppendToReply(client, ARCHKE_SIMPLE_STRING_PREFIX, strlen(ARCHKE_SIMPLE_STRING_PREFIX));
    rchkAppendToReply(client, msg->bytes, msg->size);
    rchkAppendToReply(client, ARCHKE_DELIMETER, strlen(ARCHKE_DELIMETER));
}

/*
    SET <key> <val>
    Response (simple string): +OK\r\n
*/
void setCommand(RchkClient* client) {
    // 1.
    RchkArrayElement* key = &client->in[1];
    RchkArrayElement* value = &client->in[2];

    void* keyDup = rchkDuplicate(key->bytes, key->size);
    void* valueDup = rchkDuplicate(value->bytes, value->size);

    if (rchkKVStorePut(kvstore, keyDup, key->size, valueDup, value->size) < 0) {
        // TODO: write better error error handling
        rchkExitFailure("'set' operation failed");
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
    RchkArrayElement* key = &client->in[1];

    RchkKVValue* value = rchkKVStoreGet(kvstore, key->bytes, key->size);

    if (value != NULL) {
        rchkAppendToReply(client, ARCHKE_BINARY_STRING_PREFIX, strlen(ARCHKE_BINARY_STRING_PREFIX));
        rchkAppendIntegerToReply(client, value->size);
        rchkAppendToReply(client, ARCHKE_DELIMETER, strlen(ARCHKE_DELIMETER));
        rchkAppendToReply(client, value->value, value->size);
    } else {
        rchkAppendToReply(client, ARCHKE_NULL, strlen(ARCHKE_NULL));
    }
}

/*
    DELETE <key>
    Response (simple string): +OK\r\n
*/
void rchkDelFreeKeyValue(char* key, int keySize, void* value, int valueSize) {
    rchkFreeDuplicate(key, keySize);
    rchkFreeDuplicate(value, valueSize);
}

void delCommand(RchkClient* client) {
    // 1.
    RchkArrayElement* key = &client->in[1];

    rchkKVStoreDelete2(kvstore, key->bytes, key->size, rchkDelFreeKeyValue);

    // 2.
    rchkAppendToReply(client, ":1\r\n", strlen(":1\r\n"));
}

