#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "archke_server.h"
#include "archke_error.h"
#include "archke_kvstore.h"
#include "archke_expire.h"
#include "archke_time.h"

#define	SIGINT		2	/* Interactive attention signal.  */
#define	SIGTERM		15	/* Termination request.  */

// #define ARCHKE_SERVER_CRON_DEFAULT_HZ 10
#define ARCHKE_SERVER_CRON_DEFAULT_HZ 5
#define ARCHKE_SERVER_NOT_SHUTDOWN 0
#define ARCHKE_SERVER_SHUTDOWN 1
#define ARCHKE_SERVER_ACTIVE_EXP_CURSOR_INIT 0

#define ARCHKE_ELEMENTS_ARRAY_MAX_SIZE 256
#define ARCHKE_ELEMENTS_MEMORY_MAX_SIZE (16*1024) /* 16 kb - default query buffer size */

#define ARCHKE_BSAR_ARRAY 0 // start state
#define ARCHKE_BSAR_ARRAY_SIZE 1
#define ARCHKE_BSAR_ELEMENT 2
#define ARCHKE_BSAR_ELEMENT_SIZE 3
#define ARCHKE_BSAR_ELEMENT_DATA 4
#define ARCHKE_BSAR_DONE 5 // end state

#define ARCHKE_MAX_BINARY_SIZE_CHARS 128

#define INCREMENTAL_REHASHING_TIME_THRESHOLD 1 // 1 millisecond

typedef struct RchkActiveExpiryScanData {
	uint64_t now;
} RchkActiveExpiryScanData;

RchkServer server; // Global server config

void setupSignalHandlers(void);

void rchkServerInit() {
	char* errorMessage = NULL;

	// serverCron() will be called 'ARCHKE_SERVER_CRON_DEFAULT_HZ' times per second if no
	// request will come and if it will finish in 1000/ARCHKE_SERVER_CRON_DEFAULT_HZ milliseconds each time of course
	server.hz = ARCHKE_SERVER_CRON_DEFAULT_HZ;
	server.shutdown = ARCHKE_SERVER_NOT_SHUTDOWN;
	server.cursor = ARCHKE_SERVER_ACTIVE_EXP_CURSOR_INIT;
	server.kvstore = rchkKVStoreNew();
	if (server.kvstore == NULL) {
		errorMessage = "Db keystore creation failed";
		goto err;
	}
	// TODO: right now 512 commands is max - fix it
	server.commands = rchkKVStoreNew2(NULL, 512);
	if (server.commands == NULL) {
		errorMessage = "Commands table creation failed";
		goto err;
	}
	server.expire = rchkKVStoreNew();
	if (server.expire == NULL) {
		errorMessage = "Db keystore expire creation failed";
		goto err;
	}

	setupSignalHandlers();

	return;
err:
	if (server.kvstore != NULL) { rchkKVStoreFree(server.kvstore); }
	if (server.commands != NULL) { rchkKVStoreFree(server.commands); }
	if (server.expire != NULL) { rchkKVStoreFree(server.expire); }

	rchkExitFailure(errorMessage);
}

RchkClient* rchkClientNew(int fd) {
    RchkClient* client = NULL;
    RchkQueryArg* argv = NULL;

    // will contain raw data/bytes from client
    char* queryBuff = malloc(ARCHKE_ELEMENTS_MEMORY_MAX_SIZE * sizeof(char));
    if (queryBuff == NULL) {
        goto client_create_err;
    }
	memset(queryBuff, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);

	// each command consists of elements/tokens. These elements are
	// parsed into an array of bulk/binary strings before passed further.
	// First element (element of index 0) is always a command's name
    argv = malloc(ARCHKE_ELEMENTS_ARRAY_MAX_SIZE * sizeof(RchkQueryArg));
	if (argv == NULL) {
		goto client_create_err;
	}
	for (int i=0; i<ARCHKE_ELEMENTS_ARRAY_MAX_SIZE; i++) {
		argv[i].size = 0;
		argv[i].filled = 0;
		argv[i].bytes = NULL;
	}

	// here we create client itself
    client = malloc(sizeof(RchkClient));
    if (client == NULL) {
        goto client_create_err;
    }

    client->fd = fd;

    client->queryParserState = ARCHKE_BSAR_ARRAY;
    client->queryBuff = queryBuff;
    client->queryBuffCap = ARCHKE_ELEMENTS_MEMORY_MAX_SIZE;
    client->queryBuffLen = 0;
	client->queryBuffPos = 0;
    
    client->argv = argv;
    client->argi = 0;
    client->argc = 0;

	client->reply = NULL;
	client->replyTail = NULL;
	client->replyRemaining = NULL; // not yet written response elements

    return client;

client_create_err:
    if (queryBuff != NULL) free(queryBuff);
    if (argv != NULL) free(argv);

    return NULL;
}

void rchkClientResetQueryParserState(RchkClient* client) {
	client->queryParserState = ARCHKE_BSAR_ARRAY;
}

void rchkClientResetQueryBufferState(RchkClient* client) {
	client->queryBuffLen = 0;
	client->queryBuffPos = 0;
}

void rchkClientResetArgv(RchkClient* client) {
	RchkQueryArg* argv = client->argv;
	for (int i=0; i<client->argc; i++) {
		free(argv[i].bytes);
		argv[i].size = 0;
		argv[i].filled = 0;
		argv[i].bytes = NULL;
	}
	client->argi = 0;
    client->argc = 0;
}

static void rchkClientResetReplyList(RchkClient* client) {
	RchkReplyBlock* current = client->reply;
	while (current != NULL) {
		RchkReplyBlock* next = current->next;
		free(current->bytes);
		free(current);
		current = next;
	}
	client->reply = NULL;
	client->replyTail = NULL;
	client->replyRemaining = NULL;
}

void rchkClientReset(RchkClient* client) {
	rchkClientResetQueryParserState(client);
	rchkClientResetQueryBufferState(client);
	rchkClientResetArgv(client);
	rchkClientResetReplyList(client);
}

void rchkClientFree(RchkClient* client) {
    free(client->queryBuff);
	rchkClientResetArgv(client);
	rchkClientResetReplyList(client);
    free(client->argv);
    free(client->reply);
    free(client);
}

int rchkAppendToReply(RchkClient* client, char* data, int dataSize) {
	RchkReplyBlock* element = (RchkReplyBlock*) malloc(sizeof(RchkReplyBlock));
	if (element == NULL) {
		return -1;
	}
	element->size = dataSize;
	element->next = NULL;
	element->bytes = (char*) malloc(dataSize);
	if (element->bytes == NULL) {
		return -1;
	}
	memcpy(element->bytes, data, element->size);

	if (!client->reply) {
		client->reply = element;
		client->replyTail = element;
	} else {
		client->replyTail->next = element;
		client->replyTail = element;
	}

	return 0;
}

int rchkAppendIntegerToReply(RchkClient* client, int data) {
	// 1. integer to string
	char integer[ARCHKE_MAX_BINARY_SIZE_CHARS] = { 0 };
	
	if (snprintf(integer, ARCHKE_MAX_BINARY_SIZE_CHARS, "%d", data) < 0) {
		return -1;
	}

	// 2.
	if (rchkAppendToReply(client, integer, strlen(integer)) < 0) {
		return -1;
	}

	return 0;
}

int rchkProcessQueryBuffer(RchkClient* client) {
	if (client->queryBuffLen == 0) {
		return 0; 
	}

    RchkQueryArg* arg = NULL;
	int digit = 0;
	int idx=0;

	for (idx=client->queryBuffPos; idx < client->queryBuffLen; idx++) {
		char currentByte = client->queryBuff[idx];

		switch(client->queryParserState) {
			case ARCHKE_BSAR_ARRAY:
				if (currentByte == '*') { 
					client->queryParserState = ARCHKE_BSAR_ARRAY_SIZE;
				} else { 
					client->queryParserState = ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ARRAY_SIZE:
				if (currentByte == '\r') continue;
				if (currentByte == '\n') {
					if (client->argc > 0) {
						client->queryParserState = ARCHKE_BSAR_ELEMENT;
						continue;
					}

					client->queryParserState = ARCHKE_BSAR_DONE;
				}

				digit = currentByte - '0';

				if (0 <= digit && digit <= 9) {
					client->argc = client->argc * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BSAR_ELEMENT:
				if (currentByte == '$') { 
					client->queryParserState = ARCHKE_BSAR_ELEMENT_SIZE;
				} else { 
					client->queryParserState = ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ELEMENT_SIZE:
				arg = &client->argv[client->argi];

				if (currentByte == '\r') continue;
				if (currentByte == '\n') {
					if (arg->size > 0) {
						arg->bytes = malloc(arg->size);
						if (arg->bytes == NULL) {
							rchkExitFailure("Cannot alloc memory for input array element data");
						}
						client->queryParserState = ARCHKE_BSAR_ELEMENT_DATA;
						continue;
					}

					client->argi++;

					if (client->argi < client->argc) {
						client->queryParserState = ARCHKE_BSAR_ELEMENT;
					} else {
						client->queryParserState = ARCHKE_BSAR_DONE;
					}

					continue;
				}

				digit = currentByte - '0';

				if (0 <= digit && digit <= 9) {
					arg->size = arg->size * 10 + digit;
				}

				break;
			case ARCHKE_BSAR_ELEMENT_DATA:
				arg = &client->argv[client->argi];

				arg->bytes[arg->filled] = currentByte;
				arg->filled++;

				if (arg->filled == arg->size) {
					client->argi++;
					if (client->argi < client->argc) {
						client->queryParserState = ARCHKE_BSAR_ELEMENT;
					} else {
						client->queryParserState = ARCHKE_BSAR_DONE;
						goto read_done;
					}
				}

				break;
			case ARCHKE_BSAR_DONE: 
				goto read_done;
			default:
				rchkExitFailure("Unknown query parse state");
		}
	}

read_done:
	return idx + 1; // the amount of elements processed
}

int rchkIsCompleteCommandReceived(RchkClient* client) {
	return client->queryParserState == ARCHKE_BSAR_DONE;
}

static void shutdownSignalHandler(int sig) {
	server.shutdown = ARCHKE_SERVER_SHUTDOWN;
}

void setupSignalHandlers(void) {
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = shutdownSignalHandler;
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
}


void activeExpirePrintKeyDelete(char* key, int keySize) {
	char buffer[keySize + 1];
	memcpy(buffer, key, keySize);
	buffer[keySize] = '\0';

	printf("Key expired [ key : %s, key size : %i ]\n", buffer, keySize);
}


void activeExpiryCallback(char* key, int keySize, void* value, int valueSize, void* privdata) {
	RchkActiveExpiryScanData* data = privdata;
	uint64_t now = data->now;
	uint64_t* when = value;

	if (now <= *when) { return; }

	// activeExpirePrintKeyDelete(key, keySize);

	rchkKVStoreDelete(server.kvstore, key, keySize);
	rchkRemoveExpireTime(key, keySize);
}

uint64_t rchkIncrementalRehashing(RchkKVStore* kvstore, uint64_t thresholdUs) {
	rchkKVStoreRehashActivateIfNeeded(kvstore);

	uint64_t start = rchkGetMonotonicUs();
	uint64_t now = start;
	uint64_t timelimit = start + thresholdUs;

	while (rchkKVStoreRehashStep(kvstore)) {
		// printf("rehashing step done\n");
		now = rchkGetMonotonicUs();
		if (timelimit > now) { break; }
	}

	return now - start;
}

int serverCron(RchkEventLoop* eventLoop, RchkTimeEvent* event) {
	if (server.shutdown) {
		// TODO: close listening socket? (Apparently this allows faster restarts)
		exit(0);
	}

	// Active expire scan
	int iteration = 0;
	uint64_t timeoffset = ARCHKE_ACTIVE_EXPIRY_TIME_PERCENT * (1000/server.hz)/100;
	uint64_t now = rchkGetMonotonicUs();
	uint64_t timelimit = now + timeoffset;

	RchkActiveExpiryScanData data = { .now = now };
	// printf("time offset: %lu\n", timeoffset);
	do {
		iteration++;
		server.cursor = rchkKVStoreScan(server.expire, server.cursor, activeExpiryCallback, &data);
		/* check time limit every 16 iterations. */
		if ((iteration & 0xf) == 0) {
			now = rchkGetMonotonicUs();
			if (now >= timelimit) {
				break;
			}
		}
	} while (server.cursor > 0);

	// Incremental rehashing
	rchkIncrementalRehashing(server.kvstore, INCREMENTAL_REHASHING_TIME_THRESHOLD);
	rchkIncrementalRehashing(server.expire, INCREMENTAL_REHASHING_TIME_THRESHOLD);

	return 1000/server.hz;
}
