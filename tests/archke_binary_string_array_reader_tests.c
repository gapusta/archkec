#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_binary_string_array_reader.h"

#define BUFFER_SIZE 1024

void test1() {
	char memoryArena[BUFFER_SIZE];

	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, BUFFER_SIZE);
	if (reader == NULL) {
		perror("Test #1 failed: cannot create reader instance");
		exit(1);
	}

	rchkBinaryStringArrayReaderProcess(reader, input, strlen(input));

	if (!rchkBinaryStringArrayReaderIsDone(reader)) {
		printf("Test #1 failed: Incorrect reader state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = rchkBinaryStringArrayReaderData(reader);
	
	if (elements == NULL) {
        printf("Test #1 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = rchkBinaryStringArrayReaderDataSize(reader);

	if (elementsSize != 3) {
		printf("Test #1 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 3 || strncmp(elements[0].bytes, "SET", 3) != 0) {
		printf("Test #1 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	if (elements[1].size != 5 || strncmp(elements[1].bytes, "MYKEY", 5) != 0) {
		printf("Test #1 failed: Element #2 is incorrect\n");
		exit(-1);
	}

	if (elements[2].size != 7 || strncmp(elements[2].bytes, "MYVALUE", 7) != 0) {
		printf("Test #1 failed: Element #3 is incorrect\n");
		exit(-1);
	} 

	rchkBinaryStringArrayReaderFree(reader);
	
	printf("Test #1 passed\n");
}

int main(void) {
	test1();
}

