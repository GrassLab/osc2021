#ifndef _UTILS_H_
#define  _UTILS_H_

#include "type.h"

void delay(uint64_t);
int strcmp(const char*, const char*);
int char_2_int(char);
unsigned int str_2_int(const char*);
unsigned long pow(unsigned int,
                  unsigned int);
void memset(void*, unsigned long, char);

#endif
