#ifndef ARCHKE_MEMORY
#define ARCHKE_MEMORY

char* rchkDuplicate(const char* bytes, int size);
void  rchkFreeDuplicate(char* bytes, int size);

void rchkDelFreeKeyValue(char* key, int keySize, void* value, int valueSize);

#endif
