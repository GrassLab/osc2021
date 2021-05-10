#ifndef __TIMER_H_
#define __TIMER_H_

#define TICK_PER_INT (1 << 16)
#define CPU_HZ 19200000
#define CPU_HZ_QEMU 62500000

void enable_core_timer();
void core_timer_handler();

#endif