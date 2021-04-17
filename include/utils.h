#ifndef _UTILS_H_
#define _UTILS_H_

#include "stdint.h"

void delay(unsigned long);
void put32(unsigned long, unsigned int);
unsigned int get32(unsigned long);
void run_program();
int get_el();

#endif