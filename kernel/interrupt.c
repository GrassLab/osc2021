#include "interrupt.h"
#include "uart.h"

void timer_interrupt_entry() {
    unsigned long cntpct, cntfrq;

    asm volatile("mrs x0, cntfrq_el0;"
                 "msr cntp_tval_el0, x0;");
    asm volatile("mrs %0, cntpct_el0;":"=r"(cntpct):);
    asm volatile("mrs %0, cntfrq_el0;":"=r"(cntfrq):);

    unsigned long time = cntpct / cntfrq;

    uart_puts("[debug] Elapsed time: ");
    uart_puti(time, 10);
    uart_puts(" sec\n");
}
