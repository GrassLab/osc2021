#include "include/mini_uart.h"
#define SYS_ZERO 0
#define SYS_SET_TIMER  1
#define ESR_ISS_MASK ((1 << 25) - 1)

unsigned int get_syscall_type(unsigned int esr)
{
    return esr & ESR_ISS_MASK;
}

int syscall_handler(unsigned int spsr, unsigned int elr,
    unsigned int esr)
{
    unsigned int type;
    type = get_syscall_type(esr);
    uart_send_string("From syscall_handler\r\n");
    switch(type) {
        case SYS_ZERO:
            uart_send_string("spsr_el1: ");
            uart_send_uint(spsr);
            uart_send_string("\r\n");
            uart_send_string("elr_el1: ");
            uart_send_uint(elr);
            uart_send_string("\r\n");
            uart_send_string("esr_el1: ");
            uart_send_uint(esr);
            uart_send_string("\r\n");
            break;
        case SYS_SET_TIMER:
            break;
        default:
            uart_send_string("Error: Unknown syscall type.");
    }
    return 0;
}