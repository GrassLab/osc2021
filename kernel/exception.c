#include "uart.h"

void dumpState() {
    unsigned long spsr_el1, elr_el1, esr_el1;
    asm volatile("mrs %0, spsr_el1 \n":"=r"(spsr_el1):);
    asm volatile("mrs %0, elr_el1 \n":"=r"(elr_el1):);
    asm volatile("mrs %0, esr_el1 \n":"=r"(esr_el1):);

    uart_puts("-----dump state-----\n");
    uart_puts("SPSR_EL1: 0x");
    uart_puts_hex(spsr_el1);
    uart_puts("\n");
    uart_puts("ELR_EL1: 0x");
    uart_puts_hex(elr_el1);
    uart_puts("\n");
    uart_puts("ESR_EL1: 0x");
    uart_puts_hex(esr_el1);
    uart_puts("\n");
    uart_puts("---------------------\n");
}