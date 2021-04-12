#ifndef TIMER_H
#define TIMER_H

void core_timer_enable();

void add_timer(double time, void *callback, void *data);
void print_time(unsigned long tc);

void core_timer_handler();
// timer callback is interruptable
void _add_timer(unsigned long exp_time, void *callback, void *data);

unsigned long get_timer_cnt();
extern unsigned long timer_frq;

#endif