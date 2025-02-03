#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#define ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN -1
#define ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN -2

typedef struct RchkArrayElement {
	int filled;
	int size;
	char* bytes;
} RchkArrayElement;

typedef struct RchkResponseElement {
	int size;
	char* bytes;
	struct RchkResponseElement* next;
} RchkResponseElement;

typedef struct RchkClient {
	int fd;

	// input
	int readState;	
	char* readBuffer;
	int readBufferSize;
	int readBufferOccupied;

    RchkArrayElement* in;
	int inCount;
	int inIndex;
	
	// output
	RchkResponseElement* out;
	RchkResponseElement* tail;
	RchkResponseElement* unread;
	int unreadOffset;
} RchkClient;

RchkClient* rchkClientNew(int fd);
void rchkClientReset(RchkClient* client); // reseting client after each command
void rchkClientFree(RchkClient* client);

int rchkProcessInputQuery(RchkClient* client);
int rchkIsProcessInputQueryDone(RchkClient* client);

char* rchkDuplicate(const char* bytes, int size);

int rchkAppendToReply(RchkClient* client, char* data, int dataSize);
int rchkAppendIntegerToReply(RchkClient* client, int data); // turns integet to string and appends it to reply

#endif

