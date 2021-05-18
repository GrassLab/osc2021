#include "time.h"
#include "../lib/uart.h"
#include "../lib/string.h"

void timer_handler()
{
    unsigned long cntpct, cntfrq;

    asm volatile("mrs x0, cntfrq_el0");
    asm volatile("add x0, x0, x0");
    asm volatile("msr cntp_tval_el0, x0");
    
    asm volatile("mrs %0, cntpct_el0":"=r"(cntpct));
    asm volatile("mrs %0, cntfrq_el0":"=r"(cntfrq));
    
    uart_puts("Timer:");
    uart_puts_i(cntpct / cntfrq);
    uart_puts("\n\n");
}