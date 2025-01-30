#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"

#define SIMPLE_STRING_PREFIX "+"
#define SIMPLE_STRING_SUFFIX "\r\n"

KVStore* commands;

void initCommands() {
    commands = rchkKVStoreNew();

    rchkKVStorePut(commands, "ECHO", 4, echoCommand);
}

KVStore* getCommands() { return commands; }

/*
    ECHO <msg>
    Response (simple string): +<msg>\r\n
*/
void echoCommand(RchkClient* client) {
    RchkArrayElement* msg = &client->in[1]; 
    appendToReply(client, SIMPLE_STRING_PREFIX, strlen(SIMPLE_STRING_PREFIX));
    appendToReply(client, msg->bytes, msg->size);
    appendToReply(client, SIMPLE_STRING_SUFFIX, strlen(SIMPLE_STRING_SUFFIX));
}

