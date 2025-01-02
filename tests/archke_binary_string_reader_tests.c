#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_binary_string_reader.h"

#define BUFFER_SIZE 1024

void test1() {
	char* input = "$5\r\nhello";
	char* expected = "hello";
    
	RchkBinaryStringReader* reader = rchkBinaryStringReaderNew(BUFFER_SIZE);

	rchkBinaryStringReaderProcess(reader, input, strlen(input));

	char* binary = rchkBinaryStringReaderData(reader);

    if (binary == NULL) {
        printf("Test #1 failed: NULL\n");
		exit(-1);
    }

	if (strncmp(binary, expected, strlen(expected)) != 0) {
		printf("Test #1 failed\n");
		exit(-1);
	} 

	rchkBinaryStringReaderFree(reader);
	
	printf("Test #1 passed\n");
}

void test2() {
	char* input1 = "$5\r\nhel";
	char* input2 = "lo";
    char* expected = "hello";	
		
	RchkBinaryStringReader* reader = rchkBinaryStringReaderNew(BUFFER_SIZE);

	rchkBinaryStringReaderProcess(reader, input1, strlen(input1));
	rchkBinaryStringReaderProcess(reader, input2, strlen(input2));

    char* binary = rchkBinaryStringReaderData(reader);

    if (binary == NULL) {
        printf("Test #2 failed: NULL\n");
		exit(-1);
    }

	if (strncmp(binary, expected, strlen(expected)) != 0) {
		printf("Test #2 failed\n");
		exit(-1);
	} 

	rchkBinaryStringReaderFree(reader);
	
	printf("Test #2 passed\n");
}

void test3() {
	char* input1 = "$31\r";
	char* input2 = "\nCatch me if y";
	char* input3 = "ou can, Mr. Holme";
	char* input4 = "s";
        
	char* expected = "Catch me if you can, Mr. Holmes";	
		
	RchkBinaryStringReader* reader = rchkBinaryStringReaderNew(BUFFER_SIZE);

	rchkBinaryStringReaderProcess(reader, input1, strlen(input1));
	rchkBinaryStringReaderProcess(reader, input2, strlen(input2));
    rchkBinaryStringReaderProcess(reader, input3, strlen(input3));
    rchkBinaryStringReaderProcess(reader, input4, strlen(input4));

    char* binary = rchkBinaryStringReaderData(reader);

    if (binary == NULL) {
        printf("Test #3 failed: NULL\n");
		exit(-1);
    }

	if (strncmp(binary, expected, strlen(expected)) != 0) {
		printf("Test #3 failed\n");
		exit(-1);
	} 

	rchkBinaryStringReaderFree(reader);
	
	printf("Test #3 passed\n");
}

void test4() {
	char* input = "$0\r\n";
		
	RchkBinaryStringReader* reader = rchkBinaryStringReaderNew(BUFFER_SIZE);

	rchkBinaryStringReaderProcess(reader, input, strlen(input));

	char* binary = rchkBinaryStringReaderData(reader);

    if (binary == NULL) {
        printf("Test #4 failed: NULL\n");
		exit(-1);
    }

	if (binary != 0) {
		printf("Test #4 failed\n");
		exit(-1);
	} 

	rchkBinaryStringReaderFree(reader);
	
	printf("Test #4 passed\n");
}

/*
// 'clear' operation test
void test5() {
	char* input1 = "+hello\r\n";
	char* expected1 = "hello";	
	
	char* input2 = "+hey\r\n";
	char* expected2 = "hey";

	RchkStringReader* reader = rchkStringReaderNew(BUFFER_SIZE);

	rchkStringReaderProcess(reader, input1, strlen(input1));

	char* result = rchkStringReaderData(reader);
	if (strcmp(result, expected1) != 0) {
		printf("Test #5 failed\n");
		exit(-1);
	}

	rchkStringReaderClear(reader);

	rchkStringReaderProcess(reader, input2, strlen(input2));

	result = rchkStringReaderData(reader);
	if (strcmp(result, expected2) != 0) {
		printf("Test #5 failed\n");
		exit(-1);
	}

	rchkStringReaderFree(reader);
	
	printf("Test #5 passed\n");
}
*/

int main(void) {
	test1();
	test2();
	test3();
	test4();
	// test5();
}

