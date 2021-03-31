#ifndef MYSTRING_H
#define MYSTRING

int strcmp(char* a, char* b);
int strncmp(char* a, char* b, unsigned long n);
char *memcpy(char *dest, char *src, unsigned long n);
void memset(char* a, unsigned int value, unsigned int size);

#endif
