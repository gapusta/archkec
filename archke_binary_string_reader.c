#include <stdlib.h>
#include <string.h>
#include "archke_binary_string_reader.h"

#define ARCHKE_BINARY_STRING_READER_START 0
#define ARCHKE_BINARY_STRING_READER_READ_SIZE  1
#define ARCHKE_BINARY_STRING_READER_READ_BYTES  2
#define ARCHKE_BINARY_STRING_READER_DONE  3

RchkBinaryStringReader* rchkBinaryStringReaderNew(int capacity) {
    RchkBinaryStringReader* reader = (RchkBinaryStringReader*) malloc(sizeof(RchkBinaryStringReader));
	if (reader == NULL) {
		return NULL;
	}

	RchkBinaryString* string = malloc(sizeof(RchkBinaryString) + capacity);
    if (string == NULL) {
    	free(reader);
		return NULL;
	}
    string->capacity = capacity;
    string->size = 0;
    string->data = (char*) (string + sizeof(RchkBinaryString));

	reader->state = ARCHKE_BINARY_STRING_READER_START;
	reader->index = 0;
	reader->string = string;
	
	return reader;	
}

int rchkBinaryStringReaderProcess(RchkBinaryStringReader* reader, char* bytes, int occupied) {
	RchkBinaryString* string = reader->string;

	for (int idx=0; idx<occupied; idx++) {
		char current = bytes[idx];

		switch(reader->state) {
			case ARCHKE_BINARY_STRING_READER_START:
				if (current == '$') { 
					reader->state = ARCHKE_BINARY_STRING_READER_READ_SIZE;
				} else { 
					reader->state = ARCHKE_BINARY_STRING_READER_ERROR_EXPECTED_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BINARY_STRING_READER_READ_SIZE:
				if (current == '\r') continue;
				if (current == '\n') {
					reader->state = ARCHKE_BINARY_STRING_READER_READ_BYTES;
					continue;
				}

				int digit = current - '0';

				if (0 <= digit && digit <= 9) {
					string->size = string->size * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BINARY_STRING_READER_READ_BYTES:
				if (reader->index == string->size) {
					reader->state = ARCHKE_BINARY_STRING_READER_DONE;
					continue;
				}

				string->data[reader->index] = current;
				reader->index++;

				break;
			case ARCHKE_BINARY_STRING_READER_DONE: 
				break;
		}
	}

	return 0;
}

int rchkBinaryStringReaderIsDone(RchkBinaryStringReader* reader) {
	return reader->state == ARCHKE_BINARY_STRING_READER_DONE;
}

int rchkBinaryStringReaderDataSize(RchkBinaryStringReader* reader) {
	return reader->index;
};

char* rchkBinaryStringReaderData(RchkBinaryStringReader* reader) {
	return reader->string->data;
}

void rchkBinaryStringReaderFree(RchkBinaryStringReader* reader) {
	free(reader->string);
	free(reader);
}

#undef ARCHKE_BINARY_STRING_READER_START
#undef ARCHKE_BINARY_STRING_READER_READ_SIZE
#undef ARCHKE_BINARY_STRING_READER_READ_BYTES
#undef ARCHKE_BINARY_STRING_READER_DONE

