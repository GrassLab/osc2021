#include "uart.h"
#include "util.h"
#include "timer.h"

/*
*	get_excute_time
*   
*   cntpct_el0: The timer’s current count.
*   cntfrq_el0: the frequency of the timer
*
*   excute time = cntpct_el0 / cntfrq_el0
*/
unsigned long long get_excute_time() 
{
	unsigned long long cntpct_el0, cntfrq_el0;
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
	
	return cntpct_el0 / cntfrq_el0;
}

/*
*	set_next_timeout
*   
*   cntp_tval_el0: (cntp_cval_el0 - cntpct_el0). You can use it to set an expired timer after the current timer count.
*   cntfrq_el0: the frequency of the timer
*
*   we set cntp_cval_el0 = timer frequency * second = next interrupt timeout
*/
void set_next_timeout(unsigned int second) 
{
	unsigned long cntfrq_el0;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
	asm volatile("msr cntp_tval_el0, %0" : : "r"(cntfrq_el0 * second));
}

/*
*	core time interrupt handle   
*   
*   1. set next timeout = 2s later
*   2. print excute time
*/
void core_timer_handle()
{
	set_next_timeout(2);
	
	uart_putstr("seconds after booting: ");
	
	char buf[16] = {0};
	unsignedlonglongToStr(get_excute_time(), buf);
	uart_putstr(buf);	
	uart_putstr("seconds\n");
}