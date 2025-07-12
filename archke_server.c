#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_server.h"
#include "archke_error.h"

// #define ARCHKE_SERVER_CRON_DEFAULT_HZ 10
#define ARCHKE_SERVER_CRON_DEFAULT_HZ 1

#define ARCHKE_ELEMENTS_ARRAY_MAX_SIZE 256
#define ARCHKE_ELEMENTS_MEMORY_MAX_SIZE 1024

#define ARCHKE_BSAR_ARRAY 0 // start state
#define ARCHKE_BSAR_ARRAY_SIZE 1
#define ARCHKE_BSAR_ELEMENT 2
#define ARCHKE_BSAR_ELEMENT_SIZE 3
#define ARCHKE_BSAR_ELEMENT_DATA 4
#define ARCHKE_BSAR_DONE 5 // end state

#define ARCHKE_MAX_BINARY_SIZE_CHARS 128

RchkServer server; // Global server config

void rchkServerInit() {
	server.hz = ARCHKE_SERVER_CRON_DEFAULT_HZ;
}

RchkClient* rchkClientNew(int fd) {
    RchkClient* client = NULL;
    RchkArrayElement* commandElements = NULL;
    char* readBuffer = NULL;

    // will contain raw data/bytes from client
    readBuffer = malloc(ARCHKE_ELEMENTS_MEMORY_MAX_SIZE * sizeof(char));
    if (readBuffer == NULL) {
        goto client_create_err;
    }
	memset(readBuffer, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);

	// each command consists of elements/tokens. These elements are
	// parsed into an array of bulk/binary strings before passed further.
	// First element (element of index 0) is always a command's name
    commandElements = malloc(ARCHKE_ELEMENTS_ARRAY_MAX_SIZE * sizeof(RchkArrayElement));
	if (commandElements == NULL) {
		goto client_create_err;
	}
	for (int i=0; i<ARCHKE_ELEMENTS_ARRAY_MAX_SIZE; i++) {
		commandElements[i].size = 0;
		commandElements[i].filled = 0;
		commandElements[i].bytes = NULL;
	}

	// here we create client itself
    client = malloc(sizeof(RchkClient));
    if (client == NULL) {
        goto client_create_err;
    }

    client->fd = fd;

    client->readState = ARCHKE_BSAR_ARRAY;
    client->readBuffer = readBuffer;
    client->readBufferSize = ARCHKE_ELEMENTS_MEMORY_MAX_SIZE;
	client->readBufferOccupied = 0;
    
    client->commandElements = commandElements;
    client->commandElementsCurrentIndex = 0;
    client->commandElementsCount = 0;

	client->responseElements = NULL;
	client->responseElementsTail = NULL;
	client->responseElementsUnwritten = NULL; // not yet written response elements

    return client;

client_create_err:
    if (readBuffer != NULL) free(readBuffer);
    if (commandElements != NULL) free(commandElements);

    return NULL;
}

static void rchkClearClientCommandElementsList(RchkClient* client) {
	RchkArrayElement* commandElements = client->commandElements;
	for (int i=0; i<client->commandElementsCount; i++) {
		free(commandElements[i].bytes);
		commandElements[i].size = 0;
		commandElements[i].filled = 0;
		commandElements[i].bytes = NULL;
	}
	client->commandElementsCurrentIndex = 0;
    client->commandElementsCount = 0;
}

static void rchkClearClientOutputList(RchkClient* client) {
	RchkResponseElement* current = client->responseElements;
	while (current != NULL) {
		RchkResponseElement* next = current->next;
		free(current->bytes);
		free(current);
		current = next;
	}
	client->responseElements = NULL;
	client->responseElementsTail = NULL;
	client->responseElementsUnwritten = NULL;
}

void rchkClientResetInputOnly(RchkClient* client, int bytesProcessed) {
	int bytesLeft = client->readBufferOccupied - bytesProcessed;
		
	client->readState = ARCHKE_BSAR_ARRAY;		
	client->readBufferOccupied = bytesLeft;
	memmove(client->readBuffer, client->readBuffer + bytesProcessed, bytesLeft);
	rchkClearClientCommandElementsList(client);
}

void rchkClientReset(RchkClient* client) {
	client->readState = ARCHKE_BSAR_ARRAY;
	client->readBufferOccupied = 0;
	memset(client->readBuffer, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);
	rchkClearClientCommandElementsList(client);
	rchkClearClientOutputList(client);
}

void rchkClientFree(RchkClient* client) {
    free(client->readBuffer);
	rchkClearClientCommandElementsList(client);
	rchkClearClientOutputList(client);
    free(client->commandElements);
    free(client->responseElements);
    free(client);
}

char* rchkDuplicate(const char* bytes, int size) {
	void* dup = malloc(size);
    if (dup == NULL) {
        // TODO: write better error error handling
        rchkExitFailure("duplication operation failed: malloc");
    }
	memcpy(dup, bytes, size);
    return dup;
}

void rchkFreeDuplicate(char* bytes, int size) {
	free(bytes);
}

int rchkAppendToReply(RchkClient* client, char* data, int dataSize) {
	RchkResponseElement* element = (RchkResponseElement*) malloc(sizeof(RchkResponseElement));
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

	if (!client->responseElements) {
		client->responseElements = element;
		client->responseElementsTail = element;
	} else {
		client->responseElementsTail->next = element;
		client->responseElementsTail = element;
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

int rchkProcessReadBuffer(RchkClient* client) {
	if (client->readBufferOccupied == 0) { 
		return 0; 
	}

    RchkArrayElement* currentElement = NULL;
	int digit = 0;
	int idx=0;

	for (idx=0; idx < client->readBufferOccupied; idx++) {
		char currentByte = client->readBuffer[idx];

		switch(client->readState) {
			case ARCHKE_BSAR_ARRAY:
				if (currentByte == '*') { 
					client->readState = ARCHKE_BSAR_ARRAY_SIZE;
				} else { 
					client->readState = ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ARRAY_SIZE:
				if (currentByte == '\r') continue;
				if (currentByte == '\n') {
					if (client->commandElementsCount > 0) {
						client->readState = ARCHKE_BSAR_ELEMENT;
						continue;
					}

					client->readState = ARCHKE_BSAR_DONE;
				}

				digit = currentByte - '0';

				if (0 <= digit && digit <= 9) {
					client->commandElementsCount = client->commandElementsCount * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BSAR_ELEMENT:
				if (currentByte == '$') { 
					client->readState = ARCHKE_BSAR_ELEMENT_SIZE;
				} else { 
					client->readState = ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ELEMENT_SIZE:
				currentElement = &client->commandElements[client->commandElementsCurrentIndex];

				if (currentByte == '\r') continue;
				if (currentByte == '\n') {
					if (currentElement->size > 0) {
						currentElement->bytes = malloc(currentElement->size);
						if (currentElement->bytes == NULL) {
							rchkExitFailure("Cannot alloc memory for input array element data");
						}
						client->readState = ARCHKE_BSAR_ELEMENT_DATA;						
						continue;
					}

					client->commandElementsCurrentIndex++;

					if (client->commandElementsCurrentIndex < client->commandElementsCount) {
						client->readState = ARCHKE_BSAR_ELEMENT;
					} else {
						client->readState = ARCHKE_BSAR_DONE;
					}

					continue;
				}

				digit = currentByte - '0';

				if (0 <= digit && digit <= 9) {
					currentElement->size = currentElement->size * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BSAR_ELEMENT_DATA:
				currentElement = &client->commandElements[client->commandElementsCurrentIndex];

				currentElement->bytes[currentElement->filled] = currentByte;
				currentElement->filled++;

				if (currentElement->filled == currentElement->size) {
					client->commandElementsCurrentIndex++;
					if (client->commandElementsCurrentIndex < client->commandElementsCount) {
						client->readState = ARCHKE_BSAR_ELEMENT;
					} else {
						client->readState = ARCHKE_BSAR_DONE;
						goto read_done;
					}
				}

				break;
			case ARCHKE_BSAR_DONE: 
				goto read_done;
		}
	}

read_done:
	return idx + 1; // the amount of elements processed
}

int rchkIsCompleteCommandReceived(RchkClient* client) {
	return client->readState == ARCHKE_BSAR_DONE;
}

int serverCron(RchkEventLoop* eventLoop, RchkTimeEvent* event) {
	printf("Server cron call...\n");

	return 1000/server.hz;
}
