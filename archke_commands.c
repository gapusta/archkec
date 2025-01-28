#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"

#define ARCHKE_HEADER_SIZE_MAX 32

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
    // 1.
    RchkResponseElement* first = malloc(sizeof(RchkResponseElement));
    RchkResponseElement* second = malloc(sizeof(RchkResponseElement));
    RchkResponseElement* third = malloc(sizeof(RchkResponseElement));
    if (first == NULL || second == NULL || third == NULL) {
        rchkExitFailure("Echo command memory allocation failure #1");
    }
    
    // 2. prefix
    first->size = strlen(SIMPLE_STRING_PREFIX);
    first->bytes = malloc(first->size);
    if (first->bytes == NULL) {
        rchkExitFailure("Echo command memory allocation failure #2");
    }
    memcpy(first->bytes, SIMPLE_STRING_PREFIX, first->size);
    
    // 3. body
    RchkArrayElement* body = &client->in[1];
    second->size = body->size;
    second->bytes = malloc(body->size);
    if (second->bytes == NULL) {
        rchkExitFailure("Echo command memory allocation failure #3");
    }
    memcpy(second->bytes, body->bytes, second->size);
    
    // 4. suffix
    third->size = strlen(SIMPLE_STRING_SUFFIX);
    third->bytes = malloc(third->size);
    if (third->bytes == NULL) {
        rchkExitFailure("Echo command memory allocation failure #4");
    }
    memcpy(third->bytes, SIMPLE_STRING_SUFFIX, third->size);
    
    // 5.
    client->out = first; first->next = second; second->next = third; third->next = NULL;
    
    return;
}

