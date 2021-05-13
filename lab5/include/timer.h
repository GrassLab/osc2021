#ifndef __TIMER_H_
#define __TIMER_H_

#ifdef RPI3
#define CPU_HZ 19200000LL
#else
#define CPU_HZ 62500000LL
#endif

#define TIMER_HZ 1000LL
#define MSEC_PER_SEC 1000LL
#define MSEC_PER_TICK (MSEC_PER_SEC / TIMER_HZ)
#define TICK_PER_INT (CPU_HZ / TIMER_HZ)
#define MS(n) (n / MSEC_PER_TICK)

void enable_core_timer();
void core_timer_handler();
size_t get_jiffies();

#endif