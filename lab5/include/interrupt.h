#ifndef __INTERRUPT__
#define __INTERRUPT__

void core_timer_enable();
void enable_irq();
void disable_irq();
void ret_from_fork();

#endif