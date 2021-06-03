#include "uart.h"
#include "interrupt_entry.h"
#include "irq_handler.h"

//extern IRQ_Handler (*irq_handlers)[];
extern IRQ_Handler irq_handlers[];

void invalid_entry() {
    uart_put_str("invaild handler.\n");
    while(1);
}

void el0_sync_entry() {
    unsigned long spsr_el1, elr_el1, esr_el1;
    asm volatile (
        "mrs %[spsr_el1], spsr_el1\n\t"
        "mrs %[elr_el1], elr_el1\n\t"
        "mrs %[esr_el1], esr_el1\n\t"
        : [spsr_el1] "=r" (spsr_el1), [elr_el1] "=r" (elr_el1), [esr_el1] "=r" (esr_el1)
        :
        :
    );

    uart_put_str("spsr_el1:\t");
    uart_put_addr(spsr_el1);
    uart_put_str("\nelr_el1:\t");
    uart_put_addr(elr_el1);
    uart_put_str("\nesr_el1:\t");
    uart_put_addr(esr_el1);
    uart_put_str("\n");
}

void irq_entry() {
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