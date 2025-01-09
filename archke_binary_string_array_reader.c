#include <stdlib.h>
#include "archke_binary_string_array_reader.h"

#define ARCHKE_BSAR_ELEMENTS_MAX_SIZE 256

#define ARCHKE_BSAR_ARRAY 0 // start state
#define ARCHKE_BSAR_ARRAY_SIZE 1
#define ARCHKE_BSAR_ELEMENT 2
#define ARCHKE_BSAR_ELEMENT_SIZE 3
#define ARCHKE_BSAR_ELEMENT_DATA 4
#define ARCHKE_BSAR_DONE 5 // end state

RchkBinaryStringArrayReader* rchkBinaryStringArrayReaderNew(char* buffer, int bufferSize) {
	RchkBinaryStringArrayReader* reader = malloc(sizeof(RchkBinaryStringArrayReader));
	if (reader == NULL) {
		return NULL;
	}
	
	RchkArrayElement* elements = malloc(ARCHKE_BSAR_ELEMENTS_MAX_SIZE * sizeof(RchkArrayElement));
	if (elements == NULL) {
		free(reader);
		return NULL;
	}

	for (int i=0; i<ARCHKE_BSAR_ELEMENTS_MAX_SIZE; i++) {
		elements[i].bytes = NULL;
		elements[i].size = 0;
	}

	reader->state = ARCHKE_BSAR_ARRAY;
	
	reader->buffer = buffer;
	reader->bufferOffset = 0;
	reader->bufferSize = bufferSize;
	
	reader->elements = elements;
	reader->elementsIndex = 0;
	reader->elementsCount = 0;

	return reader;
}

int rchkBinaryStringArrayReaderProcess(RchkBinaryStringArrayReader* reader, char* bytes, int occupied) {
	RchkArrayElement* element = NULL;
	int digit = 0;

	for (int idx=0; idx<occupied; idx++) {
		char current = bytes[idx];

		switch(reader->state) {
			case ARCHKE_BSAR_ARRAY:
				if (current == '*') { 
					reader->state = ARCHKE_BSAR_ARRAY_SIZE;
				} else { 
					reader->state = ARCHKE_BSAR_ERROR_EXPECTED_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ARRAY_SIZE:
				if (current == '\r') continue;
				if (current == '\n') {
					if (reader->elementsCount > 0) {
						reader->state = ARCHKE_BSAR_ELEMENT;
						continue;
					}

					reader->state = ARCHKE_BSAR_DONE;
				}

				digit = current - '0';

				if (0 <= digit && digit <= 9) {
					reader->elementsCount = reader->elementsCount * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BSAR_ELEMENT:
				if (current == '$') { 
					reader->state = ARCHKE_BSAR_ELEMENT_SIZE;
				} else { 
					reader->state = ARCHKE_BSAR_ERROR_EXPECTED_ELEMENT_START_SIGN;
					return -1;		
				}
				
				break;
			case ARCHKE_BSAR_ELEMENT_SIZE:
				element = reader->elements + reader->elementsIndex;

				if (current == '\r') continue;
				if (current == '\n') {
					if (element->size > 0) {
						element->bytes = reader->buffer + reader->bufferOffset;
						reader->state = ARCHKE_BSAR_ELEMENT_DATA;						
						continue;
					}

					reader->elementsIndex++;
					if (reader->elementsIndex < reader->elementsCount) {
						reader->state = ARCHKE_BSAR_ELEMENT;
					} else {
						reader->state = ARCHKE_BSAR_DONE;
					}

					continue;
				}

				digit = current - '0';

				if (0 <= digit && digit <= 9) {
					element->size = element->size * 10 + digit;
					continue;
				}

				break;
			case ARCHKE_BSAR_ELEMENT_DATA:
				element = reader->elements + reader->elementsIndex;

				reader->buffer[reader->bufferOffset] = current;
				reader->bufferOffset++;

				if (reader->buffer + reader->bufferOffset == element->bytes + element->size) {
					reader->elementsIndex++;
					if (reader->elementsIndex < reader->elementsCount) {
						reader->state = ARCHKE_BSAR_ELEMENT;
					} else {
						reader->state = ARCHKE_BSAR_DONE;
					}
				}

				break;
			case ARCHKE_BSAR_DONE: 
				break;
		}
	}

	return 0;
}

int rchkBinaryStringArrayReaderIsDone(RchkBinaryStringArrayReader* reader) {
	return reader->state == ARCHKE_BSAR_DONE;
}

RchkArrayElement* rchkBinaryStringArrayReaderData(RchkBinaryStringArrayReader* reader) {
	return reader->elements;
}

int rchkBinaryStringArrayReaderDataSize(RchkBinaryStringArrayReader* reader) {
	return reader->elementsCount;
}

void rchkBinaryStringArrayReaderFree(RchkBinaryStringArrayReader* reader) {
	free(reader->elements);
	free(reader);
}

