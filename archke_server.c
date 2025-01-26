#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "archke_server.h"
#include "archke_error.h"

#define ARCHKE_ELEMENTS_ARRAY_MAX_SIZE 256
#define ARCHKE_ELEMENTS_MEMORY_MAX_SIZE 1024

#define ARCHKE_BSAR_ARRAY 0 // start state
#define ARCHKE_BSAR_ARRAY_SIZE 1
#define ARCHKE_BSAR_ELEMENT 2
#define ARCHKE_BSAR_ELEMENT_SIZE 3
#define ARCHKE_BSAR_ELEMENT_DATA 4
#define ARCHKE_BSAR_DONE 5 // end state

RchkClient* rchkClientNew(int fd) {
    RchkClient* client = NULL;
    RchkArrayElement* in = NULL;
    char* readBuffer = NULL;

    // input memory arena and array
    readBuffer = malloc(ARCHKE_ELEMENTS_MEMORY_MAX_SIZE * sizeof(char));
    if (readBuffer == NULL) {
        goto client_create_err;
    }
	memset(readBuffer, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);

    in = malloc(ARCHKE_ELEMENTS_ARRAY_MAX_SIZE * sizeof(RchkArrayElement));
	if (in == NULL) {
		goto client_create_err;
	}
	for (int i=0; i<ARCHKE_ELEMENTS_ARRAY_MAX_SIZE; i++) {
		in[i].size = 0;
		in[i].filled = 0;
		in[i].bytes = NULL;
	}

    client = malloc(sizeof(RchkClient));
    if (client == NULL) {
        goto client_create_err;
    }

    client->fd = fd;

    client->readState = ARCHKE_BSAR_ARRAY;
    client->readBuffer = readBuffer;
    client->readBufferSize = ARCHKE_ELEMENTS_MEMORY_MAX_SIZE;
	client->readBufferOccupied = 0;
    
    client->in = in;
    client->inIndex = 0;
    client->inCount = 0;

	client->out = NULL;
	client->unread = NULL;
	client->unreadOffset = 0;

    return client;

client_create_err:
    if (readBuffer != NULL) free(readBuffer);
    if (in != NULL) free(in);
    if (client != NULL) free(client);

    return NULL;
}

int rchkProcessInputQuery(RchkClient* client) {
    RchkArrayElement* currentElement = NULL;
	int digit = 0;

	for (int idx=0; idx < client->readBufferOccupied; idx++) {
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
					if (client->inCount > 0) {
						client->readState = ARCHKE_BSAR_ELEMENT;
						continue;
					}

					client->readState = ARCHKE_BSAR_DONE;
				}

				digit = currentByte - '0';

				if (0 <= digit && digit <= 9) {
					client->inCount = client->inCount * 10 + digit;
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
				currentElement = &client->in[client->inIndex];

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

					client->inIndex++;

					if (client->inIndex < client->inCount) {
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
				currentElement = &client->in[client->inIndex];

				currentElement->bytes[currentElement->filled] = currentByte;
				currentElement->filled++;

				if (currentElement->filled == currentElement->size) {
					client->inIndex++;
					if (client->inIndex < client->inCount) {
						client->readState = ARCHKE_BSAR_ELEMENT;
					} else {
						client->readState = ARCHKE_BSAR_DONE;
					}
				}

				break;
			case ARCHKE_BSAR_DONE: 
				break;
		}
	}

	return 0;
}

int rchkIsProcessInputQueryDone(RchkClient* client) {
	return client->readState == ARCHKE_BSAR_DONE;
}

void rchkClientFree(RchkClient* client) {
    free(client->readBuffer);
    free(client->in); // TODO: free 'in' array elements as well
    free(client->out); // TODO: free 'out' array elements as well
    free(client);
}

