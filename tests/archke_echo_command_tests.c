#include <stdio.h>
#include <string.h>
#include "archke_server.h"
#include "archke_commands.h"

void test1() {
    RchkArrayElement in[2] = {
        { .size = 4, .bytes = "ECHO" },
        { .size = 5, .bytes = "hello" }
    };
    RchkClient client = { 
        .in = in,
        .out = NULL
    };

    echoCommand(&client);

    RchkResponseElement* first = client.out;

    if (first == NULL || first->next == NULL) {
        printf("Test #1: check #1 failed\n");
        return;
    }

    if (first->size != 4) { // $5\r\n
        printf("Test #1: check #2 failed\n");
        return;
    }

    if (strncmp(first->bytes, "$5\r\n", first->size) != 0) {
        printf("Test #1: check #3 failed\n");
        return;
    }

    RchkResponseElement* next = first->next;

    if (next->size != 5) {
        printf("Test #1: check #4 failed\n");
        return;
    }

    if (strncmp(next->bytes, "hello", next->size) != 0) {
        printf("Test #1: check #5 failed\n");
        return;
    }

    printf("Test #1 passed\n");
}

void test2() {
    RchkArrayElement in[2] = {
        { .size = 4, .bytes = "ECHO" },
        { .size = 52, .bytes = "Aequeosalinocalcalinoceraceoaluminosocupreovitriolic" } // this is an actual word
    };
    RchkClient client = { 
        .in = in,
        .out = NULL
    };

    echoCommand(&client);

    RchkResponseElement* first = client.out;

    if (first == NULL) {
        printf("Test #2: check #1 failed\n");
        return;
    }

    if (first->size != 5) { // $52\r\n
        printf("Test #2: check #2 failed\n");
        return;
    }

    if (strncmp(first->bytes, "$52\r\n", first->size) != 0) {
        printf("Test #2: check #3 failed\n");
        return;
    }

    RchkResponseElement* next = first->next;

    if (next->size != 52) {
        printf("Test #2: check #4 failed\n");
        return;
    }

    if (strncmp(next->bytes, "Aequeosalinocalcalinoceraceoaluminosocupreovitriolic", next->size) != 0) {
        printf("Test #2: check #5 failed\n");
        return;
    }

    printf("Test #2 passed\n");
}

int main(void) {
    test1();
    test2();
    return 0;
}

