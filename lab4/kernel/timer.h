#ifndef TIMER_H
#define TIMER_H

extern void core_timer_enable();
extern void core_timer_disable();

unsigned long long get_excute_time();
void set_next_timeout(unsigned int second);
void core_timer_handle();

#endif