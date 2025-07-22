#ifndef ARCHKE_EXPIRE
#define ARCHKE_EXPIRE

#include <stdint.h>

void rchkInitExpire();
int rchkSetExpireTime(char* key, int keySize, uint64_t timeout);
void rchkRemoveExpireTime(char* key, int keySize);
int rchkIsExpired(char* key, int keySize);

#endif
