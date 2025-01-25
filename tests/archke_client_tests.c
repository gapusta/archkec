#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_server.h"

#define MEMORY_ARENA_SIZE 1024

void test1() {
	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE"; int inputSize = strlen(input);

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #1 failed: cannot create client instance");
		exit(1);
	}

	client->readBufferRead = inputSize; memcpy(client->readBuffer, input, inputSize); rchkProcessInputQuery(client);

	if (!rchkIsProcessInputQueryDone(client)) {
		printf("Test #1 failed: Incorrect client state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = client->in;
	
	if (elements == NULL) {
        printf("Test #1 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = client->inCount;

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

	rchkClientFree(client);
	
	printf("Test #1 passed\n");
}

void test2() {
	// [*][1][\r][\n][$3][\r\n][S][ET]
	char* input1 = "*"; int inputSize1 = strlen(input1);
	char* input2 = "1"; int inputSize2 = strlen(input2);
	char* input3 = "\r"; int inputSize3 = strlen(input3);
	char* input4 = "\n"; int inputSize4 = strlen(input4);
	char* input5 = "$3"; int inputSize5 = strlen(input5);
	char* input6 = "\r\n"; int inputSize6 = strlen(input6);
	char* input7 = "S"; int inputSize7 = strlen(input7);
	char* input8 = "ET"; int inputSize8 = strlen(input8);

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #2 failed: cannot create client instance");
		exit(1);
	}

	client->readBufferRead = inputSize1; memcpy(client->readBuffer, input1, inputSize1); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize2; memcpy(client->readBuffer, input2, inputSize2); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize3; memcpy(client->readBuffer, input3, inputSize3); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize4; memcpy(client->readBuffer, input4, inputSize4); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize5; memcpy(client->readBuffer, input5, inputSize5); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize6; memcpy(client->readBuffer, input6, inputSize6); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize7; memcpy(client->readBuffer, input7, inputSize7); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize8; memcpy(client->readBuffer, input8, inputSize8); rchkProcessInputQuery(client);

	if (!rchkIsProcessInputQueryDone(client)) {
		printf("Test #2 failed: Incorrect client state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = client->in;
	
	if (elements == NULL) {
        printf("Test #2 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = client->inCount;

	if (elementsSize != 1) {
		printf("Test #2 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 3 || strncmp(elements[0].bytes, "SET", 3) != 0) {
		printf("Test #2 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	rchkClientFree(client);
	
	printf("Test #2 passed\n");
}

void test3() {
	// [*][1][\r][\n][$1][1][\r\n][DIST][INGUISH]
	char* input1 = "*"; int inputSize1 = strlen(input1);
	char* input2 = "1"; int inputSize2 = strlen(input2);
	char* input3 = "\r"; int inputSize3 = strlen(input3);
	char* input4 = "\n"; int inputSize4 = strlen(input4);
	char* input5 = "$1"; int inputSize5 = strlen(input5);
	char* input6 = "1"; int inputSize6 = strlen(input6);
	char* input7 = "\r\n"; int inputSize7 = strlen(input7);
	char* input8 = "DIST"; int inputSize8 = strlen(input8);
	char* input9 = "INGUISH"; int inputSize9 = strlen(input9);

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #3 failed: cannot create client instance");
		exit(1);
	}

	client->readBufferRead = inputSize1; memcpy(client->readBuffer, input1, inputSize1); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize2; memcpy(client->readBuffer, input2, inputSize2); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize3; memcpy(client->readBuffer, input3, inputSize3); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize4; memcpy(client->readBuffer, input4, inputSize4); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize5; memcpy(client->readBuffer, input5, inputSize5); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize6; memcpy(client->readBuffer, input6, inputSize6); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize7; memcpy(client->readBuffer, input7, inputSize7); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize8; memcpy(client->readBuffer, input8, inputSize8); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize9; memcpy(client->readBuffer, input9, inputSize9); rchkProcessInputQuery(client);

	if (!rchkIsProcessInputQueryDone(client)) {
		printf("Test #3 failed: Incorrect client state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = client->in;
	
	if (elements == NULL) {
        printf("Test #3 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = client->inCount;

	if (elementsSize != 1) {
		printf("Test #3 failed: Incorrect elements amount\n");
		exit(-1);
	}

	if (elements[0].size != 11 || strncmp(elements[0].bytes, "DISTINGUISH", 11) != 0) {
		printf("Test #3 failed: Element #1 is incorrect\n");
		exit(-1);
	}

	rchkClientFree(client);
	
	printf("Test #3 passed\n");
}

void test4() {
	// [*][3][\r][\n][$1][1][\r\n][DIST][INGUISH][$][5\r\n][MY][KEY][$7\r][\n][MYVALU][E]
	char* input1 = "*"; int inputSize1 = strlen(input1);
	char* input2 = "3"; int inputSize2 = strlen(input2);
	char* input3 = "\r"; int inputSize3 = strlen(input3);
	char* input4 = "\n"; int inputSize4 = strlen(input4);
	char* input5 = "$1"; int inputSize5 = strlen(input5);
	char* input6 = "1"; int inputSize6 = strlen(input6);
	char* input7 = "\r\n"; int inputSize7 = strlen(input7);
	char* input8 = "DIST"; int inputSize8 = strlen(input8);
	char* input9 = "INGUISH"; int inputSize9 = strlen(input9);
	char* input10 = "$"; int inputSize10 = strlen(input10);
	char* input11 = "5\r\n"; int inputSize11 = strlen(input11);
	char* input12 = "MY"; int inputSize12 = strlen(input12);
	char* input13 = "KEY"; int inputSize13 = strlen(input13);
	char* input14 = "$7\r"; int inputSize14 = strlen(input14);
	char* input15 = "\n"; int inputSize15 = strlen(input15);
	char* input16 = "MYVALU"; int inputSize16 = strlen(input16);
	char* input17 = "E"; int inputSize17 = strlen(input17);

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #4 failed: cannot create client instance");
		exit(1);
	}

	client->readBufferRead = inputSize1; memcpy(client->readBuffer, input1, inputSize1); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize2; memcpy(client->readBuffer, input2, inputSize2); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize3; memcpy(client->readBuffer, input3, inputSize3); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize4; memcpy(client->readBuffer, input4, inputSize4); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize5; memcpy(client->readBuffer, input5, inputSize5); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize6; memcpy(client->readBuffer, input6, inputSize6); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize7; memcpy(client->readBuffer, input7, inputSize7); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize8; memcpy(client->readBuffer, input8, inputSize8); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize9; memcpy(client->readBuffer, input9, inputSize9); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize10; memcpy(client->readBuffer, input10, inputSize10); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize11; memcpy(client->readBuffer, input11, inputSize11); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize12; memcpy(client->readBuffer, input12, inputSize12); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize13; memcpy(client->readBuffer, input13, inputSize13); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize14; memcpy(client->readBuffer, input14, inputSize14); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize15; memcpy(client->readBuffer, input15, inputSize15); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize16; memcpy(client->readBuffer, input16, inputSize16); rchkProcessInputQuery(client);
	client->readBufferRead = inputSize17; memcpy(client->readBuffer, input17, inputSize17); rchkProcessInputQuery(client);
	
	if (!rchkIsProcessInputQueryDone(client)) {
		printf("Test #4 failed: Incorrect client state: expected 'DONE'\n");
		exit(-1);
	}

	RchkArrayElement* elements = client->in;
	
	if (elements == NULL) {
        printf("Test #4 failed: NULL\n");
		exit(-1);
    }
	
	int elementsSize = client->inCount;

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

	rchkClientFree(client);
	
	printf("Test #4 passed\n");
}

int main(void) {
	test1();
	test2();
	test3();
	test4();
}

