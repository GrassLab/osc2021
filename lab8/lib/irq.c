#include "../include/irq.h"
#include "../include/uart.h"
#include "../include/task.h"

void _timer_handler(void){

	unsigned long cntpct,cntfrq;
    int cnt;

    asm volatile("mrs x0, cntfrq_el0	\n");
	asm volatile("asr x0, x0, 8		\n");
	asm volatile("msr cntp_tval_el0, x0	\n");
	//asm volatile("mrs %0, cntpct_el0	\n":"=r"(cntpct):);
	//asm volatile("mrs %0, cntfrq_el0	\n":"=r"(cntfrq):);

	//cnt=cntpct/cntfrq;
	//uart_printf("--------------------\n");
	//uart_printf("Time Elapsed: %ds\n",cnt);
	//uart_printf("--------------------\n");
    threadSchedule();
}
