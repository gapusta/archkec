#ifndef ARCHKE_BINARY_STRING_ARRAY_READER
#define ARCHKE_BINARY_STRING_ARRAY_READER

#define ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN -1
#define ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN -2

typedef struct RchkArrayElement {
	char* bytes;
	int size;
} RchkArrayElement;

typedef struct RchkBinaryStringArrayReader { // Archke simple string reader 
	int state;
	char* buffer; // internal memory arena, array elements will point to parts of this memory chunk
	int bufferSize;
	int bufferOffset;
    RchkArrayElement* elements;
	int elementsCount;
	int elementsIndex;
} RchkBinaryStringArrayReader;

RchkBinaryStringArrayReader* rchkBinaryStringArrayReaderNew(char* buffer, int bufferSize);
void rchkBinaryStringArrayReaderFree(RchkBinaryStringArrayReader* reader);

int rchkBinaryStringArrayReaderProcess(RchkBinaryStringArrayReader* reader, char* bytes, int occupied);
int rchkBinaryStringArrayReaderIsDone(RchkBinaryStringArrayReader* reader);

RchkArrayElement* rchkBinaryStringArrayReaderData(RchkBinaryStringArrayReader* reader);
int rchkBinaryStringArrayReaderDataSize(RchkBinaryStringArrayReader* reader);

#endif

