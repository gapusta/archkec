#include <stdlib.h>
#include <string.h>
#include "archke_server.h"

#define ARCHKE_ELEMENTS_MAX_SIZE 256
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
    RchkArrayElement* out = NULL;
    char* inBuffer = NULL;
    char* outBuffer = NULL;

    // input memory arena and array
    inBuffer = malloc(ARCHKE_ELEMENTS_MEMORY_MAX_SIZE * sizeof(char));
    if (inBuffer == NULL) {
        goto client_create_err;
    }
	memset(inBuffer, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);

    in = malloc(ARCHKE_ELEMENTS_MAX_SIZE * sizeof(RchkArrayElement));
	if (in == NULL) {
		goto client_create_err;
	}
	for (int i=0; i<ARCHKE_ELEMENTS_MAX_SIZE; i++) {
		in[i].size = 0;
		in[i].bytes = NULL;
	}

    // output memory arena and array
    outBuffer = malloc(ARCHKE_ELEMENTS_MEMORY_MAX_SIZE * sizeof(char));
    if (inBuffer == NULL) {
        goto client_create_err;
    }
	memset(outBuffer, 0, ARCHKE_ELEMENTS_MEMORY_MAX_SIZE);

    out = malloc(ARCHKE_ELEMENTS_MAX_SIZE * sizeof(RchkArrayElement));
	if (in == NULL) {
		goto client_create_err;
	}
	for (int i=0; i<ARCHKE_ELEMENTS_MAX_SIZE; i++) {
		out[i].size = 0;
		out[i].bytes = NULL;
	}

    client = malloc(sizeof(RchkClient));
    if (client == NULL) {
        goto client_create_err;
    }

    client->fd = fd;

    client->readState = ARCHKE_BSAR_ARRAY;
    client->readBuffer = inBuffer;
    client->readBufferPos = 0;
    client->readBufferSize = ARCHKE_ELEMENTS_MEMORY_MAX_SIZE;
    
    client->in = in;
    client->inIndex = 0;
    client->inCount = 0;

    client->writeBuffer = outBuffer;
    client->writeBufferPos = 0;
    client->writeBufferSize = ARCHKE_ELEMENTS_MEMORY_MAX_SIZE;

    client->out = out;
    client->outIndex = 0;
    client->outCount = 0;

    return client;

client_create_err:
    if (inBuffer != NULL) free(inBuffer);
    if (in != NULL) free(in);
    if (outBuffer != NULL) free(outBuffer);
    if (out != NULL) free(out);
    if (client != NULL) free(client);

    return NULL;
}

int rchkProcessInputQuery(RchkClient* client, char* bytes, int occupied) {
    RchkArrayElement* currentElement = NULL;
	int digit = 0;

	for (int idx=0; idx<occupied; idx++) {
		char currentByte = bytes[idx];

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
						currentElement->bytes = &client->readBuffer[client->readBufferPos];
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
				client->readBuffer[client->readBufferPos] = currentByte;
				client->readBufferPos++;

				currentElement = &client->in[client->inIndex];

				if (client->readBuffer + client->readBufferPos == currentElement->bytes + currentElement->size) {
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
    free(client->writeBuffer);    
    free(client->in);
    free(client->out);
    free(client);
}

