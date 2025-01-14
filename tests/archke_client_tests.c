#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_server.h"

#define MEMORY_ARENA_SIZE 1024

void test1() {
	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE";

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #1 failed: cannot create client instance");
		exit(1);
	}

	rchkProcessInputQuery(client, input, strlen(input));

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
	char* input1 = "*";
	char* input2 = "1";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$3";
	char* input6 = "\r\n";
	char* input7 = "S";
	char* input8 = "ET";

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #2 failed: cannot create client instance");
		exit(1);
	}

	rchkProcessInputQuery(client, input1, strlen(input1));
	rchkProcessInputQuery(client, input2, strlen(input2));
	rchkProcessInputQuery(client, input3, strlen(input3));
	rchkProcessInputQuery(client, input4, strlen(input4));
	rchkProcessInputQuery(client, input5, strlen(input5));
	rchkProcessInputQuery(client, input6, strlen(input6));
	rchkProcessInputQuery(client, input7, strlen(input7));
	rchkProcessInputQuery(client, input8, strlen(input8));

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
	char* input1 = "*";
	char* input2 = "1";
	char* input3 = "\r";
	char* input4 = "\n";
	char* input5 = "$1";
	char* input6 = "1";
	char* input7 = "\r\n";
	char* input8 = "DIST";
	char* input9 = "INGUISH";

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #3 failed: cannot create client instance");
		exit(1);
	}

	rchkProcessInputQuery(client, input1, strlen(input1));
	rchkProcessInputQuery(client, input2, strlen(input2));
	rchkProcessInputQuery(client, input3, strlen(input3));
	rchkProcessInputQuery(client, input4, strlen(input4));
	rchkProcessInputQuery(client, input5, strlen(input5));
	rchkProcessInputQuery(client, input6, strlen(input6));
	rchkProcessInputQuery(client, input7, strlen(input7));
	rchkProcessInputQuery(client, input8, strlen(input8));
	rchkProcessInputQuery(client, input9, strlen(input9));

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

	RchkClient* client = rchkClientNew(-1);
	if (client == NULL) {
		perror("Test #4 failed: cannot create client instance");
		exit(1);
	}

	rchkProcessInputQuery(client, input1, strlen(input1));
	rchkProcessInputQuery(client, input2, strlen(input2));
	rchkProcessInputQuery(client, input3, strlen(input3));
	rchkProcessInputQuery(client, input4, strlen(input4));
	rchkProcessInputQuery(client, input5, strlen(input5));
	rchkProcessInputQuery(client, input6, strlen(input6));
	rchkProcessInputQuery(client, input7, strlen(input7));
	rchkProcessInputQuery(client, input8, strlen(input8));
	rchkProcessInputQuery(client, input9, strlen(input9));
	rchkProcessInputQuery(client, input10, strlen(input10));
	rchkProcessInputQuery(client, input11, strlen(input11));
	rchkProcessInputQuery(client, input12, strlen(input12));
	rchkProcessInputQuery(client, input13, strlen(input13));
	rchkProcessInputQuery(client, input14, strlen(input14));
	rchkProcessInputQuery(client, input15, strlen(input15));
	rchkProcessInputQuery(client, input16, strlen(input16));
	rchkProcessInputQuery(client, input17, strlen(input17));
	
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

