#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "archke_tests.h"

char* rchkTestName = "default";

void rchkTestSetName(char* name) {
    rchkTestName = name;
}

void rchkAssertNull(void* p, char* tag) {
    if (p != NULL) {
        printf("TEST [%s]. Tag : %s. Error: value is NOT null\n", rchkTestName, tag);
        exit(1);
    }
}

void rchkAssertNotNull(void* p, char* tag) {
    if (p == NULL) {
        printf("TEST [%s]. Tag : %s. Error: value is NULL\n", rchkTestName, tag);
        exit(1);
    }
}

void rchkAssertEqualsInt(int expected, int actual, char* tag) {
    if (expected != actual) {
        printf("TEST [%s]. Tag : %s. Error: Values [ expected = %d, actual = %d ] are not equal\n", rchkTestName, tag, expected, actual);
        exit(1);
    }
}

void rchkAssertEqualsContent(char* expected, char* actual, size_t n, char* tag) {
    if (strncmp(expected, actual, n) != 0) {
        printf("TEST [%s]. Tag : %s. Error: strings are not content equal\n", rchkTestName, tag);
        exit(1);
    }
}

