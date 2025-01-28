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
    if (first == NULL) {
        printf("Test #1: check #1 failed\n");
        return;
    }
    if (first->size != 1) {
        printf("Test #1: check #2 failed\n");
        return;
    }
    if (strncmp(first->bytes, "+", first->size) != 0) {
        printf("Test #1: check #3 failed\n");
        return;
    }

    RchkResponseElement* second = first->next;
    if (second == NULL) {
        printf("Test #1: check #4 failed\n");
        return;
    }
    if (second->size != 5) {
        printf("Test #1: check #5 failed\n");
        return;
    }
    if (strncmp(second->bytes, "hello", second->size) != 0) {
        printf("Test #1: check #6 failed\n");
        return;
    }

    RchkResponseElement* third = second->next;
    if (third == NULL) {
        printf("Test #1: check #7 failed\n");
        return;
    }
    if (third->next != NULL) {
        printf("Test #1: check #8 failed\n");
        return;
    }
    if (third->size != 2) {
        printf("Test #1: check #9 failed\n");
        return;
    }
    if (strncmp(third->bytes, "\r\n", third->size) != 0) {
        printf("Test #1: check #10 failed\n");
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
        printf("Test #1: check #1 failed\n");
        return;
    }
    if (first->size != 1) {
        printf("Test #1: check #2 failed\n");
        return;
    }
    if (strncmp(first->bytes, "+", first->size) != 0) {
        printf("Test #1: check #3 failed\n");
        return;
    }

    RchkResponseElement* second = first->next;
    if (second == NULL) {
        printf("Test #1: check #4 failed\n");
        return;
    }
    if (second->size != 52) {
        printf("Test #1: check #5 failed\n");
        return;
    }
    if (strncmp(second->bytes, "Aequeosalinocalcalinoceraceoaluminosocupreovitriolic", second->size) != 0) {
        printf("Test #1: check #6 failed\n");
        return;
    }

    RchkResponseElement* third = second->next;
    if (third == NULL) {
        printf("Test #1: check #7 failed\n");
        return;
    }
    if (third->next != NULL) {
        printf("Test #1: check #8 failed\n");
        return;
    }
    if (third->size != 2) {
        printf("Test #1: check #9 failed\n");
        return;
    }
    if (strncmp(third->bytes, "\r\n", third->size) != 0) {
        printf("Test #1: check #10 failed\n");
        return;
    }

    printf("Test #2 passed\n");
}

int main(void) {
    test1();
    test2();
    return 0;
}

