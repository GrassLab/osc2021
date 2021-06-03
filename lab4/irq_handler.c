#include "irq_handler.h"
#include "uart.h"

void unconfig(void) {
    uart_put_str("this handler is not config.\n");
}

void core_timer_handler(void) {
    unsigned long timer_count, timer_freq = 0;
    asm volatile (
        "mrs %[timer_freq], cntfrq_el0\n\t"
        "mrs %[timer_count], cntpct_el0\n\t"
        "mov x10, %[timer_freq], lsl 1\n\t"
        "msr cntp_tval_el0, x10\n\t"
        : [timer_count] "=r" (timer_count), [timer_freq] "=r" (timer_freq)
        : "1" (timer_freq)
        :
    );
    unsigned long time = timer_count / timer_freq;
    uart_put_str("time: ");
    uart_put_ulong(time);
    uart_put_str("\n");
}

IRQ_Handler irq_handlers[MAX_IRQ_HANDLER] = {
    unconfig,
    core_timer_handler,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
    unconfig,
};