#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"

#define ARCHKE_HEADER_SIZE_MAX 32

static char* prefixSimpleString = "+";

static char* suffixSimpleString = "\r\n";

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
    RchkResponseElement* out = malloc(3 * sizeof(RchkResponseElement));
    if (out == NULL) {
        rchkExitFailure("Echo command memory allocation failure #1");
    }
    
    // 2. prefix
    out[0].size = strlen(prefixSimpleString);
    out[0].bytes = prefixSimpleString;
    
    // 3. body
    RchkArrayElement* body = &client->in[1];
    out[1].size = body->size;
    out[1].bytes = malloc(body->size);
    if (out[1].bytes == NULL) {
        free(out);
        rchkExitFailure("Echo command memory allocation failure #2");
    }
    memcpy(out[1].bytes, body->bytes, body->size);
    
    // 4. suffix
    out[2].bytes = suffixSimpleString;
    out[2].size = strlen(suffixSimpleString);

    // 5.
    client->out = &out[0];
    out[0].next = &out[1];
    out[1].next = &out[2];
    out[2].next = NULL;
    
    return;
}

