#include "include/irq.h"
#include "include/uart.h"
#include "include/util.h"
#include "include/scheduler.h"

void unconfig() {
    uart_put_str("this handler is not config.\n");
}

void core_timer_handler() {
    set_time();
}

void el0_irq_handler() {
    unsigned int first_level_irq_pendding;
    asm volatile (
        "ldr %[first_level_irq_pendding], [%[core0_interrupt_source]]"
        : [first_level_irq_pendding] "=r" (first_level_irq_pendding)
        : [core0_interrupt_source] "r" (CORE0_INTERRUPT_SOURCE)
        :
    );

    for (int i = 0; i < 32; i++) {
        if (first_level_irq_pendding & (1 << i))
            irq_handlers[i]();
    }
}

void el1_irq_handler() {
    unsigned int first_level_irq_pendding;
    asm volatile (
        "ldr %[first_level_irq_pendding], [%[core0_interrupt_source]]"
        : [first_level_irq_pendding] "=r" (first_level_irq_pendding)
        : [core0_interrupt_source] "r" (CORE0_INTERRUPT_SOURCE)
        :
    );

    for (int i = 0; i < 32; i++) {
        if (first_level_irq_pendding & (1 << i))
            irq_handlers[i]();
    }
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