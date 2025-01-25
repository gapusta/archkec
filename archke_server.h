#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#define ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN -1
#define ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN -2

typedef struct RchkArrayElement {
	int filled;
	int size;
	char* bytes;
} RchkArrayElement;

typedef struct RchkClient {
	int fd;

	// input
	int readState;	
	char* readBuffer;
	int readBufferSize;
	int readBufferPos;
	int readBufferRead;

    RchkArrayElement* in;
	int inCount;
	int inIndex;
	
	// output
	RchkArrayElement* out;
	int outCount;
	int outIndex;
	int outSent;
} RchkClient;

RchkClient* rchkClientNew(int fd);
void rchkClientFree(RchkClient* client);

int rchkProcessInputQuery(RchkClient* client);
int rchkIsProcessInputQueryDone(RchkClient* client);

#endif

