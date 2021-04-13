#ifndef __TIMER_H__
#define __TIMER_H__

#include "uart.h"
#include "system.h"
#define TIME_FREQ 0x03b9aca0

extern unsigned int get_current_timer_cnt();
void add_timer(void (*)(char *));
void timer_print_msg(char *);
void init_timer_queue();

typedef struct timer_event {
    unsigned int registerTime, execTime;
    char msg[20];
    struct timer_event* next;
} timer_event;


#endif