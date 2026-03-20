#ifndef ARCHKE_SERVER
#define ARCHKE_SERVER

#include "archke_event_loop.h"
#include "archke_kvstore.h"

/* command argument is considered big if it is bigger than or equal to this constant */
#define ARCHKE_CMD_BIG_ARG (32*1024)
/* default query buffer size */
#define ARCHKE_QUERY_BUFFER_DEFAULT_SIZE (16*1024)

#define ARCHKE_ACTIVE_EXPIRY_TIME_PERCENT 25

#define ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN -1
#define ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN -2

typedef struct RchkQueryArg {
	int filled;
	int size;
	char* bytes;
} RchkQueryArg;

typedef struct RchkResponseElement {
	int size;
	char* bytes;
	struct RchkResponseElement* next;
} RchkReplyBlock;

typedef struct RchkClient {
	int fd; /* socket fd */

	int queryParserState; /* query parsing state machine [ raw input -> argv ] */

	char* queryBuff; /* query bytes from socket */
	int queryBuffCap; /* query buffer capacity */
	int queryBuffLen; /* how much of query buffer is occupied by query bytes */
	int queryBuffPos; /* points at first unprocessed byte in query buffer */

	// any command is expected to be an array of bulk/binary strings
    RchkQueryArg* argv; /* arguments of current command */
	int argc; /* arguments count of current command */
	int argi; /* currently processed argument */

	// output
	// TODO: Make it a linked list
	RchkReplyBlock* reply;
	RchkReplyBlock* replyTail;
	RchkReplyBlock* replyRemaining; /* not yet written response elements */
} RchkClient;

typedef struct RchkServer {
	int hz;
	int shutdown;
	uint64_t cursor; /* active expire scan cursor */
	RchkKVStore* kvstore; /* stores data */
	RchkKVStore* commands; /* stores executable commands (e.g. 'SET', 'GET') */
	RchkKVStore* expire; /* stores when keys are supposed to expire */
} RchkServer;

extern RchkServer server;

void rchkServerInit();

RchkClient* rchkClientNew(int fd);
void rchkClientResetQueryBufferState(RchkClient* client);
void rchkClientResetQueryParserState(RchkClient* client);
void rchkClientResetArgv(RchkClient* client);
void rchkClientReset(RchkClient* client); // resets client after each command
void rchkClientFree(RchkClient* client);

int rchkProcessQueryBuffer(RchkClient* client);
int rchkIsCompleteCommandReceived(RchkClient* client);

int rchkAppendToReply(RchkClient* client, char* data, int dataSize);
int rchkAppendIntegerToReply(RchkClient* client, int data); // turns integer to string and appends it to reply

int serverCron(RchkEventLoop* eventLoop, RchkTimeEvent* event);

#endif
