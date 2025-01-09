#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_binary_string_array_reader.h"

#define MEMORY_ARENA_SIZE 1024

void test1() {
	char memoryArena[MEMORY_ARENA_SIZE];

	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, MEMORY_ARENA_SIZE);
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
	char memoryArena[MEMORY_ARENA_SIZE];

	// [*][1][\r][\n][$3][\r\n][S][ET]
	char* input1 = "*";
	char* input2 = "1";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$3";
	char* input6 = "\r\n";
	char* input7 = "S";
	char* input8 = "ET";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, MEMORY_ARENA_SIZE);
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
	char memoryArena[MEMORY_ARENA_SIZE];

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

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, MEMORY_ARENA_SIZE);
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

void test4() {
	char memoryArena[MEMORY_ARENA_SIZE];

	// [*][3][\r][\n][$1][1][\r\n][DIST][INGUISH][$][5\r\n][MY][KEY][$7\r][\n][MYVALU][E]
	char* input1 = "*";
	char* input2 = "3";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$1";
	char* input6 = "1";
	char* input7 = "\r\n";
	char* input8 = "DIST";
	char* input9 = "INGUISH";
	char* input10 = "$";
	char* input11 = "5\r\n";
	char* input12 = "MY";
	char* input13 = "KEY";
	char* input14 = "$7\r";
	char* input15 = "\n";
	char* input16 = "MYVALU";
	char* input17 = "E";

	RchkBinaryStringArrayReader* reader = rchkBinaryStringArrayReaderNew(memoryArena, MEMORY_ARENA_SIZE);
	if (reader == NULL) {
		perror("Test #4 failed: cannot create reader instance");
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
	rchkBinaryStringArrayReaderProcess(reader, input10, strlen(input10));
	rchkBinaryStringArrayReaderProcess(reader, input11, strlen(input11));
	rchkBinaryStringArrayReaderProcess(reader, input12, strlen(input12));
	rchkBinaryStringArrayReaderProcess(reader, input13, strlen(input13));
	rchkBinaryStringArrayReaderProcess(reader, input14, strlen(input14));
	rchkBinaryStringArrayReaderProcess(reader, input15, strlen(input15));
	rchkBinaryStringArrayReaderProcess(reader, input16, strlen(input16));
	rchkBinaryStringArrayReaderProcess(reader, input17, strlen(input17));
	
	if (!rchkBinaryStringArrayReaderIsDone(reader)) {
		printf("Test #4 failed: Incorrect reader state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = rchkBinaryStringArrayReaderData(reader);
	
	if (elements == NULL) {
        printf("Test #4 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = rchkBinaryStringArrayReaderDataSize(reader);

	if (elementsSize != 3) {
		printf("Test #4 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 11 || strncmp(elements[0].bytes, "DISTINGUISH", 11) != 0) {
		printf("Test #4 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	if (elements[1].size != 5 || strncmp(elements[1].bytes, "MYKEY", 5) != 0) {
		printf("Test #4 failed: Element #2 is incorrect\n");
		exit(-1);
	}

	if (elements[2].size != 7 || strncmp(elements[2].bytes, "MYVALUE", 7) != 0) {
		printf("Test #4 failed: Element #3 is incorrect\n");
		exit(-1);
	}

	rchkBinaryStringArrayReaderFree(reader);
	
	printf("Test #4 passed\n");
}

int main(void) {
	test1();
	test2();
	test3();
	test4();
}

