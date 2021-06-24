#ifndef _UTILS_H_
#define  _UTILS_H_

#include "type.h"

void delay(uint64_t);
size_t strlen(const char*);
int strcmp(const char*, const char*);
void strcpy(char*, const char*);
void strncpy(char*, const char*, size_t);
void strcat(char*, const char*);
int strchr(const char*, char);
int ctoi(char);
unsigned int atoi(const char*);
uint8_t* htoa(uint64_t);
unsigned long pow(unsigned int,
                  unsigned int);
void memset(void*, size_t, char);
void memcpy(void*, void*, size_t);

#endif
