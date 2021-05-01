#ifndef _VT_H_
#define _VT_H_
#include "uart.h"

extern void _load_user_program(void*, void*);
extern int get_el(void);
void print_el();

#endif
