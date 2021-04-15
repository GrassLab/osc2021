#include "exception.h"
#include "uart.h"
#include "time.h"

void log_state()
{
    unsigned long esr, elr, spsr;
    asm volatile("mrs %0, esr_el1  \n":"=r"(esr):);
    asm volatile("mrs %0, elr_el1  \n":"=r"(elr):);
    asm volatile("mrs %0, spsr_el1 \n":"=r"(spsr));

    uart_puts("--------------- State ---------------\n");
    uart_puts("SPSR: ");
    uart_puts_h(spsr);
    uart_puts("\n");
    uart_puts("ELR: ");
    uart_puts_h(elr);
    uart_puts("\n");
    uart_puts("ESR: ");
    uart_puts_h(esr);
    uart_puts("\n");
    uart_puts("-------------------------------------\n");
}

void handle_synchronous()
{
    uart_puts("Synchronous exception\n");
    log_state();
}

void handle_irq()
{
    uart_puts("IRQ exception\n");
    log_state();

    timer_handler();
}

void handle_unknown_exception()
{
    uart_puts("Error: Unknown exception!\n");
    log_state();
}