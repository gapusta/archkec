#ifndef ARCHKE_SERVER_COMMANDS
#define ARCHKE_SERVER_COMMANDS

#include "archke_server.h"
#include "archke_kvstore.h"

/*
    Command prototype is [ void cmd(RchkClient* client) ]
*/

void initKvstore();
void initCommands();

RchkKVStore* getCommands();

// Commands

void echoCommand(RchkClient* client);
void setCommand(RchkClient* client);
void getCommand(RchkClient* client);
void delCommand(RchkClient* client);

#endif

