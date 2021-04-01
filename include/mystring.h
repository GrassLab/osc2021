#ifndef MYSTRING_H
#define MYSTRING

int strcmp(char* a, char* b);
int strncmp(char* a, char* b, unsigned long n);
char *memcpy(char *dest, char *src, unsigned long n);
void memset(char* a, unsigned int value, unsigned int size);
char *itoa(int value, char *s, int base);
int atoi(char *s);
unsigned int vsprintf(char *dest, char *fmt, __builtin_va_list args);

#endif
