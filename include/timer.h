#ifndef TIMER_H
#define TIMER_H

void core_timer_enable();

void add_timer(double time, void *callback, void *data);
void print_time(void *data);

void core_timer_handler();

#endif