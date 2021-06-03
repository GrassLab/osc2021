#pragma once

#include <stddef.h>

int strcmp(const char *p1, const char *p2);
int strncmp(const char *s1, const char *s2, unsigned n);
int strlen(const char *s);
char *strstr(const char *s, const char *find);
char *strcpy(char *dst, const char *src);
char *strncpy(char *destination, const char *source, size_t len);
void strcat(char *to, const char *from);
char *strtok(char *s, const char delim);
char *split_last(char *str, char delim);
