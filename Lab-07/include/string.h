#ifndef _STRING_H
#define _STRING_H 



unsigned int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(char *str1, char *str2, int num);
int strcmp_eq(char *str1, char *str2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned long n);
char *strrchr(const char *s, char c);
char *strtok(char *s, const char delim);
void *memset(void *s, int c, unsigned long n);

#endif//_LIB_STRING_H