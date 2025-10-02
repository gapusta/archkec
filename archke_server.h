#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#include "archke_event_loop.h"
#include "archke_kvstore.h"

#define ARCHKE_ACTIVE_EXPIRY_TIME_PERCENT 25

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

	// [ raw input -> command elements ] mapping state machine
	int readState;

	// raw input bytes from socket
	char* readBuffer;
	int readBufferSize;
	int readBufferOccupied;

	// any command is expected to be an array of bulk/binary strings
    RchkArrayElement* commandElements; // elements of this command/array
	int commandElementsCount;
	int commandElementsCurrentIndex;
	
	// output
	RchkResponseElement* responseElements;
	RchkResponseElement* responseElementsTail;
	RchkResponseElement* responseElementsUnwritten; // not yet written response elements
} RchkClient;

typedef struct RchkServer {
	int hz;
	int shutdown;
	int cursor; /* active expire scan cursor */
	RchkKVStore* kvstore; /* stores data */
	RchkKVStore* commands; /* stores executable commands (e.g. 'SET', 'GET') */
	RchkKVStore* expire; /* stores when keys are supposed to expire */
} RchkServer;

extern RchkServer server;

void rchkServerInit();

RchkClient* rchkClientNew(int fd);
void rchkClientReset(RchkClient* client); // resets client after each command
void rchkClientResetInputOnly(RchkClient* client, int bytesProcessed);
void rchkClientFree(RchkClient* client);

int rchkProcessReadBuffer(RchkClient* client);
int rchkIsCompleteCommandReceived(RchkClient* client);

int rchkAppendToReply(RchkClient* client, char* data, int dataSize);
int rchkAppendIntegerToReply(RchkClient* client, int data); // turns integer to string and appends it to reply

int serverCron(RchkEventLoop* eventLoop, RchkTimeEvent* event);

#endif
