#include "mini_uart.h"
#include "stdint.h"
#include "str_tool.h"


void print_state(){
    uint64_t spsr, elr, esr;
    asm volatile("mrs %0, spsr_el1 \n":"=r"(spsr):);
    asm volatile("mrs %0, elr_el1 \n":"=r"(elr):);
    asm volatile("mrs %0, esr_el1 \n":"=r"(esr):);

    uart_puts("spsr_el1 = ");
    uart_puts(itoa(spsr, 16));
    uart_puts("\r\nelr_el1 = ");
    uart_puts(itoa(elr, 16));
    uart_puts("\r\nesr_el1 = ");
    uart_puts(itoa(esr, 16));
    uart_puts("\r\n");
}

void exception_handler(){
    print_state();
}

void non_implement_handler(){
    uart_puts("This instruction has not been implemented!\r\n");
    while(1);
}