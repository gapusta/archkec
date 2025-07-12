#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#include "archke_event_loop.h"

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
} RchkServer;

extern RchkServer server;

void rchkServerInit();

RchkClient* rchkClientNew(int fd);
void rchkClientReset(RchkClient* client); // reseting client after each command
void rchkClientResetInputOnly(RchkClient* client, int bytesProcessed);
void rchkClientFree(RchkClient* client);

int rchkProcessReadBuffer(RchkClient* client);
int rchkIsCompleteCommandReceived(RchkClient* client);

char* rchkDuplicate(const char* bytes, int size);
void  rchkFreeDuplicate(char* bytes, int size);

int rchkAppendToReply(RchkClient* client, char* data, int dataSize);
int rchkAppendIntegerToReply(RchkClient* client, int data); // turns integet to string and appends it to reply

int serverCron(RchkEventLoop* eventLoop, RchkTimeEvent* event);

#endif
