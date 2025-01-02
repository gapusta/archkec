#ifndef ARCHKE_BINARY_STRING_READER
#define ARCHKE_BINARY_STRING_READER

#define ARCHKE_BINARY_STRING_READER_ERROR_EXPECTED_START_SIGN -1

/*
 Binary String

    Pattern: $<length>\r\n<binary-data>

    "length" - the string's length (in bytes), encoded in ASCII
*/
typedef struct RchkBinaryString {
	int capacity;
	int size;
	char* data;
} RchkBinaryString;

typedef struct RchkBinaryStringReader { // Archke simple string reader 
	int state;
	int index;
	RchkBinaryString* string;
} RchkBinaryStringReader;

RchkBinaryStringReader* rchkBinaryStringReaderNew(int dataSize);
void rchkBinaryStringReaderFree(RchkBinaryStringReader* reader);

int rchkBinaryStringReaderProcess(RchkBinaryStringReader* reader, char* bytes, int occupied);
int rchkBinaryStringReaderIsDone(RchkBinaryStringReader* reader);

char* rchkBinaryStringReaderData(RchkBinaryStringReader* reader);
int rchkBinaryStringReaderDataSize(RchkBinaryStringReader* reader);

#endif

