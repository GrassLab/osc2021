#ifndef __TIMER_H__
#define __TIMER_H__

#include "uart.h"
#include "system.h"
// #define TIME_FREQ 0x03b9aca0

extern unsigned long long get_current_timer_cnt();
extern unsigned long long get_timer_freq();
void add_timer(void (*callback)(char *), char *, int);
void timer_print_msg(char *);
void init_timer_queue();
void el1_timer_irq();
void el0_timer_irq();

typedef struct timer_event
{
    unsigned long long registerTime, duration;
    char args[20];
    void (*callback)(char *);
    struct timer_event *next;
} timer_event;

#endif