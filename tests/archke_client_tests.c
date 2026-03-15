#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../archke_tests.h"
#include "../archke_server.h"

#define MEMORY_ARENA_SIZE 1024

void test1() {
	rchkTestSetName("Test #1");

	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE"; int inputSize = strlen(input);

	RchkClient* client = rchkClientNew(-1);
	rchkAssertNotNull(client, "client null check");

	client->queryBuffLen = inputSize;
	memcpy(client->queryBuff, input, inputSize);
	rchkProcessQueryBuffer(client);

	rchkAssertEqualsInt(1, rchkIsCompleteCommandReceived(client), "query finish check");

	RchkQueryArg* elements = client->argv;

	rchkAssertNotNull(elements, "elements null check");
	
	int elementsSize = client->argc;

	rchkAssertEqualsInt(3, elementsSize, "elements size check");
	rchkAssertEqualsInt(3, elements[0].size, "element #0 size check");
	rchkAssertEqualsInt(5, elements[1].size, "element #1 size check");
	rchkAssertEqualsInt(7, elements[2].size, "element #2 size check");
	
	rchkAssertEqualsContent("SET", elements[0].bytes, 3, "element #0 value check");
	rchkAssertEqualsContent("MYKEY", elements[1].bytes, 5, "element #1 value check");
	rchkAssertEqualsContent("MYVALUE", elements[2].bytes, 7, "element #2 value check");

	rchkClientFree(client);
	
	printf("Test #1 passed\n");
}

void test2() {
	rchkTestSetName("Test #2");

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
	rchkAssertNotNull(client, "client null check");

	client->queryBuffLen = inputSize1; memcpy(client->queryBuff, input1, inputSize1); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize2; memcpy(client->queryBuff, input2, inputSize2); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize3; memcpy(client->queryBuff, input3, inputSize3); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize4; memcpy(client->queryBuff, input4, inputSize4); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize5; memcpy(client->queryBuff, input5, inputSize5); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize6; memcpy(client->queryBuff, input6, inputSize6); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize7; memcpy(client->queryBuff, input7, inputSize7); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize8; memcpy(client->queryBuff, input8, inputSize8); rchkProcessQueryBuffer(client);

	rchkAssertEqualsInt(1, rchkIsCompleteCommandReceived(client), "query finish check");

	RchkQueryArg* elements = client->argv;

	rchkAssertNotNull(elements, "elements null check");

	int elementsSize = client->argc;

	rchkAssertEqualsInt(1, elementsSize, "elements size check");
	rchkAssertEqualsInt(3, elements[0].size, "element #0 size check");
	rchkAssertEqualsContent("SET", elements[0].bytes, 3, "element #0 value check");

	rchkClientFree(client);

	printf("Test #2 passed\n");
}


void test3() {
	rchkTestSetName("Test #3");

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
	rchkAssertNotNull(client, "client null check");

	client->queryBuffLen = inputSize1; memcpy(client->queryBuff, input1, inputSize1); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize2; memcpy(client->queryBuff, input2, inputSize2); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize3; memcpy(client->queryBuff, input3, inputSize3); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize4; memcpy(client->queryBuff, input4, inputSize4); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize5; memcpy(client->queryBuff, input5, inputSize5); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize6; memcpy(client->queryBuff, input6, inputSize6); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize7; memcpy(client->queryBuff, input7, inputSize7); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize8; memcpy(client->queryBuff, input8, inputSize8); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize9; memcpy(client->queryBuff, input9, inputSize9); rchkProcessQueryBuffer(client);

	rchkAssertEqualsInt(1, rchkIsCompleteCommandReceived(client), "query finish check");

	RchkQueryArg* elements = client->argv;

	rchkAssertNotNull(elements, "elements null check");

	int elementsSize = client->argc;

	rchkAssertEqualsInt(1, elementsSize, "elements size check");
	rchkAssertEqualsInt(11, elements[0].size, "element #0 size check");
	rchkAssertEqualsContent("DISTINGUISH", elements[0].bytes, 11, "element #0 value check");

	rchkClientFree(client);

	printf("Test #3 passed\n");
}

void test4() {
	rchkTestSetName("Test #4");

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
	rchkAssertNotNull(client, "client null check");

	client->queryBuffLen = inputSize1; memcpy(client->queryBuff, input1, inputSize1); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize2; memcpy(client->queryBuff, input2, inputSize2); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize3; memcpy(client->queryBuff, input3, inputSize3); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize4; memcpy(client->queryBuff, input4, inputSize4); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize5; memcpy(client->queryBuff, input5, inputSize5); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize6; memcpy(client->queryBuff, input6, inputSize6); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize7; memcpy(client->queryBuff, input7, inputSize7); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize8; memcpy(client->queryBuff, input8, inputSize8); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize9; memcpy(client->queryBuff, input9, inputSize9); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize10; memcpy(client->queryBuff, input10, inputSize10); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize11; memcpy(client->queryBuff, input11, inputSize11); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize12; memcpy(client->queryBuff, input12, inputSize12); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize13; memcpy(client->queryBuff, input13, inputSize13); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize14; memcpy(client->queryBuff, input14, inputSize14); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize15; memcpy(client->queryBuff, input15, inputSize15); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize16; memcpy(client->queryBuff, input16, inputSize16); rchkProcessQueryBuffer(client);
	client->queryBuffLen = inputSize17; memcpy(client->queryBuff, input17, inputSize17); rchkProcessQueryBuffer(client);

	rchkAssertEqualsInt(1, rchkIsCompleteCommandReceived(client), "query finish check");

	RchkQueryArg* elements = client->argv;

	rchkAssertNotNull(elements, "elements null check");

	int elementsSize = client->argc;

	rchkAssertEqualsInt(3, elementsSize, "elements size check");
	rchkAssertEqualsInt(11, elements[0].size, "element #0 size check");
	rchkAssertEqualsInt(5, elements[1].size, "element #1 size check");
	rchkAssertEqualsInt(7, elements[2].size, "element #2 size check");

	rchkAssertEqualsContent("DISTINGUISH", elements[0].bytes, 11, "element #0 value check");
	rchkAssertEqualsContent("MYKEY", elements[1].bytes, 5, "element #1 value check");
	rchkAssertEqualsContent("MYVALUE", elements[2].bytes, 7, "element #2 value check");

	rchkClientFree(client);

	printf("Test #4 passed\n");
}

void test5() {
	rchkTestSetName("Test #5");

	char* input = "*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE*3\r\n$3\r\nSET$6\r\nMYKEY2$8\r\nMYVALUE2";
	int inputSize = strlen(input);

	RchkClient* client = rchkClientNew(-1);
	rchkAssertNotNull(client, "client null check");

	client->queryBuffLen = inputSize;
	memcpy(client->queryBuff, input, inputSize);

	int processed = rchkProcessQueryBuffer(client);

	rchkAssertEqualsInt(1, rchkIsCompleteCommandReceived(client), "query finish check");

	rchkAssertEqualsInt(strlen("*3\r\n$3\r\nSET$5\r\nMYKEY$7\r\nMYVALUE"), processed, "processed bytes amount check");

	RchkQueryArg* elements = client->argv;

	rchkAssertNotNull(elements, "elements null check");

	int elementsSize = client->argc;

	rchkAssertEqualsInt(3, elementsSize, "elements size check");
	rchkAssertEqualsInt(3, elements[0].size, "element #0 size check");
	rchkAssertEqualsInt(5, elements[1].size, "element #1 size check");
	rchkAssertEqualsInt(7, elements[2].size, "element #2 size check");

	rchkAssertEqualsContent("SET", elements[0].bytes, 3, "element #0 value check");
	rchkAssertEqualsContent("MYKEY", elements[1].bytes, 5, "element #1 value check");
	rchkAssertEqualsContent("MYVALUE", elements[2].bytes, 7, "element #2 value check");

	rchkClientFree(client);

	printf("Test #5 passed\n");
}

int main(void) {
	test1();
	test2();
	test3();
	test4();
	test5();
}

