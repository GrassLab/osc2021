#include "exception.h"
#include "uart.h"

void exception_entry() {
    uart_puts("[debug] exception entry\n");

    unsigned long spsr_el1, elr_el1, esr_el1;
    asm volatile("mrs %0, spsr_el1":"=r"(spsr_el1):);
    asm volatile("mrs %0, elr_el1":"=r"(elr_el1):);
    asm volatile("mrs %0, esr_el1":"=r"(esr_el1):);

    uart_puts("[debug]    spsr_el1: 0x");
    uart_putul(spsr_el1, 16);
    uart_puts("\n");

    uart_puts("[debug]    elr_el1: 0x");
    uart_putul(elr_el1, 16);
    uart_puts("\n");

    uart_puts("[debug]    esr_el1: 0x");
    uart_putul(esr_el1, 16);
    uart_puts("\n");

    /*
    unsigned long currentel = get_current_exception_level();
    uart_puts("[debug]    currentel: ");
    uart_puti(currentel, 10);
    uart_puts("\n");
    */
}

unsigned long get_current_exception_level() {
    unsigned long current_el;
    asm volatile("mrs %0, currentel":"=r"(current_el):);
    current_el = (current_el & 0b001100) >> 2;

    // EL, bits [3:2]
    // Current Exception level. Possible values of this field are:
    //  ------------------
    //  | 00  |     EL0  |
    //  ------------------
    //  | 01  |     EL1  |
    //  ------------------
    //  | 10  |     EL2  |
    //  ------------------
    //  | 11  |     EL3  |
    //  ------------------

    return current_el;
}