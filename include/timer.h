#ifndef __TIMER_H__
#define __TIMER_H__

#include "uart.h"
#define TIME_FREQ 0x03b9aca0

extern unsigned int get_current_timer_cnt();
void add_timer(void (*)(char *));
void timer_print_msg(char *);

#endif