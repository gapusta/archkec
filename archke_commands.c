#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_commands.h"
#include "archke_error.h"

#define ARCHKE_HEADER_SIZE_MAX 32

KVStore* commands;

void initCommands() {
    commands = rchkKVStoreNew();

    rchkKVStorePut(commands, "ECHO", 4, echoCommand);
}

KVStore* getCommands() {
    return commands;
}

/*
    ECHO <msg>
    Response : $<size>\r\n<msg>
*/
void echoCommand(RchkClient* client) {
    // 1.
    RchkResponseElement* out = malloc(2 * sizeof(RchkResponseElement));
    if (out == NULL) {
        rchkExitFailure("Echo command response memory allocation failed");
    }
    
    // 2.
    char header[ARCHKE_HEADER_SIZE_MAX] = { 0 };
    if (snprintf(header, ARCHKE_HEADER_SIZE_MAX, "$%d\r\n", client->in[1].size) < 0) {
        rchkExitFailure("echo command: snprintf");
    }

    out[0].size = strlen(header);
    out[0].bytes = malloc(out[0].size);
    if (out[0].bytes == NULL) {
        free(out);
        rchkExitFailure("Echo command response memory allocation for response header failed");
    }
    memcpy(out[0].bytes, header, out[0].size);
    
    // 3.
    out[0].next = &out[1];
    out[1].size = client->in[1].size;
    out[1].bytes = malloc(out[1].size);
    if (out[1].bytes == NULL) {
        free(out[0].bytes);
        free(out);
        rchkExitFailure("Echo command response memory allocation for response body failed");
    }
    // body
    memcpy(out[1].bytes, client->in[1].bytes, out[1].size);

    client->out = out;

    return;
}

