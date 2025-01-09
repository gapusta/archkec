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

void test2() {
	char memoryArena[BUFFER_SIZE];

	// [*][1][\r][\n][$3][\r\n][S][ET]
	char* input1 = "*";
	char* input2 = "1";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$3";
	char* input6 = "\r\n";
	char* input7 = "S";
	char* input8 = "ET";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, BUFFER_SIZE);
	if (reader == NULL) {
		perror("Test #2 failed: cannot create reader instance");
		exit(1);
	}

	rchkBinaryStringArrayReaderProcess(reader, input1, strlen(input1));
	rchkBinaryStringArrayReaderProcess(reader, input2, strlen(input2));
	rchkBinaryStringArrayReaderProcess(reader, input3, strlen(input3));
	rchkBinaryStringArrayReaderProcess(reader, input4, strlen(input4));
	rchkBinaryStringArrayReaderProcess(reader, input5, strlen(input5));
	rchkBinaryStringArrayReaderProcess(reader, input6, strlen(input6));
	rchkBinaryStringArrayReaderProcess(reader, input7, strlen(input7));
	rchkBinaryStringArrayReaderProcess(reader, input8, strlen(input8));

	if (!rchkBinaryStringArrayReaderIsDone(reader)) {
		printf("Test #2 failed: Incorrect reader state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = rchkBinaryStringArrayReaderData(reader);
	
	if (elements == NULL) {
        printf("Test #2 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = rchkBinaryStringArrayReaderDataSize(reader);

	if (elementsSize != 1) {
		printf("Test #2 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 3 || strncmp(elements[0].bytes, "SET", 3) != 0) {
		printf("Test #2 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	rchkBinaryStringArrayReaderFree(reader);
	
	printf("Test #2 passed\n");
}

void test3() {
	char memoryArena[BUFFER_SIZE];

	// [*][1][\r][\n][$1][1][\r\n][DIST][INGUISH]
	char* input1 = "*";
	char* input2 = "1";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$1";
	char* input6 = "1";
	char* input7 = "\r\n";
	char* input8 = "DIST";
	char* input9 = "INGUISH";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, BUFFER_SIZE);
	if (reader == NULL) {
		perror("Test #3 failed: cannot create reader instance");
		exit(1);
	}

	rchkBinaryStringArrayReaderProcess(reader, input1, strlen(input1));
	rchkBinaryStringArrayReaderProcess(reader, input2, strlen(input2));
	rchkBinaryStringArrayReaderProcess(reader, input3, strlen(input3));
	rchkBinaryStringArrayReaderProcess(reader, input4, strlen(input4));
	rchkBinaryStringArrayReaderProcess(reader, input5, strlen(input5));
	rchkBinaryStringArrayReaderProcess(reader, input6, strlen(input6));
	rchkBinaryStringArrayReaderProcess(reader, input7, strlen(input7));
	rchkBinaryStringArrayReaderProcess(reader, input8, strlen(input8));
	rchkBinaryStringArrayReaderProcess(reader, input9, strlen(input9));

	if (!rchkBinaryStringArrayReaderIsDone(reader)) {
		printf("Test #3 failed: Incorrect reader state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = rchkBinaryStringArrayReaderData(reader);
	
	if (elements == NULL) {
        printf("Test #3 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = rchkBinaryStringArrayReaderDataSize(reader);

	if (elementsSize != 1) {
		printf("Test #3 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 11 || strncmp(elements[0].bytes, "DISTINGUISH", 11) != 0) {
		printf("Test #3 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	rchkBinaryStringArrayReaderFree(reader);
	
	printf("Test #3 passed\n");
}

int main(void) {
	test1();
	test2();
	test3();
}

