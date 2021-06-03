#include "utils.h"
#include "printf.h"
#include "sched.h"
#include "peripherals/timer.h"
unsigned int read_cntfrq(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}

void write_cntp_tval(unsigned int val)
{
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}

unsigned int read_cntp_tval(void)
{
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void ) 
{
	unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val)); // read val
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val)); // write tval
	timer_tick();
}

void enable_core_timer(void)
{
    unsigned int cntfrq;
    cntfrq = read_cntfrq();
    write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
    core_timer_enable();
    enable_irq();
}
