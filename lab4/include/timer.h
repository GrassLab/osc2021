# include "mem_addr.h"

# define CORE_TIMER_DEFAULT_FREQ  2000  // unit : ms

void print_timer(unsigned long long ms);
extern "C"
void core_timer_init();
void core_timer_enable();
void core_timer_disable();
void core_timer_interrupt_handler();
void get_core_timer_value(unsigned long long* r);
void get_core_timer_ms(unsigned long long* r);
