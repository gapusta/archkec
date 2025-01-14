#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#define ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN -1
#define ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN -2

typedef struct RchkArrayElement {
	char* bytes;
	int size;
} RchkArrayElement;

typedef struct RchkClient {
	int fd;

	int readState;	
	char* readBuffer;
	int readBufferSize;
	int readBufferPos;

    RchkArrayElement* in;
	int inCount;
	int inIndex;
	
	char* writeBuffer;
	int writeBufferSize;
	int writeBufferPos;
    
	RchkArrayElement* out;
	int outCount;
	int outIndex;
} RchkClient;

RchkClient* rchkClientNew(int fd);
void rchkClientFree(RchkClient* client);

int rchkProcessInputQuery(RchkClient* client, char* bytes, int occupied);
int rchkIsProcessInputQueryDone(RchkClient* client);

#endif

