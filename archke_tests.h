#ifndef ARCHKE_TESTS
#define ARCHKE_TESTS

#include <stdlib.h>

void rchkTestSetName(char* name);

void rchkAssertNull(void* p, char* tag);
void rchkAssertNotNull(void* p, char* tag);
void rchkAssertEqualsInt(int expected, int actual, char* tag);
void rchkAssertEqualsContent(char* expected, char* actual, size_t n, char* tag);

#endif

