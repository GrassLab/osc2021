#include "../../include/except.h"
#include "../../include/uart.h"

void _except_handler(){

    unsigned long spsr, elr, esr;
    asm volatile("mrs %0 ,spsr_el1   \n":"=r"(spsr):);
    asm volatile("mrs %0 ,elr_el1     \n":"=r"(elr):);
    asm volatile("mrs %0 ,esr_el1    \n":"=r"(esr):);
    uart_printf("spsr: 0x%x\n",spsr);
    uart_printf("elr: 0x%x\n",elr);
    uart_printf("esr: 0x%x\n",esr);
    return ;
}
