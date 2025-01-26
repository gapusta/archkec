#ifndef ARCHKE_SERVER_COMMANDS
#define ARCHKE_SERVER_COMMANDS

#include "archke_server.h"
#include "archke_kvstore.h"

/*
    Command prototype is [ void cmd(RchkClient* client) ]
*/

void initCommands();

KVStore* getCommands();

// Commands

void echoCommand(RchkClient* client);

#endif

