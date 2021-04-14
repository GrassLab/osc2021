#ifndef __UTIL_H_
#define __UTIL_H_

#define NULL ((void *)0)

typedef unsigned int size_t;

char *gets(char *);
int puts(const char *);
int putln(const char *);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, unsigned long);

#endif // __UTIL_H_
