#include "uart.h"
#include "utils.h"

void loadimg() {

    volatile unsigned char *prog;
    unsigned char code;
    unsigned int size;
    unsigned int addr;
    char s[20];

    uart_puts("Waiting for a program to be sent from the UART ...\n");
    addr = uart_getc();
    addr = addr + (uart_getc() << 8);
    addr = addr + (uart_getc() << 16);
    addr = addr + (uart_getc() << 24);
    prog = (unsigned char *) addr;
    size = uart_getc();
    size = size + (uart_getc() << 8);
    size = size + (uart_getc() << 16);
    size = size + (uart_getc() << 24);

    for (int idx = 0; idx < size; idx++)
    {
        code = uart_getc();
        prog[idx] = code;
    }
    uart_puts("raspi3 executes code at ");
    itoa(addr, s);
    uart_puts(s);
    uart_puts(", size = ");
    itoa(size, s);
    uart_puts(s);
    uart_puts(" bytes.\n");
    uart_puts("-----------------------------------------------------------");
    uart_puts("------------\n");
    for(int i = 0; i < 10; i++) {
        asm volatile("nop");
    }

    void (*jump_to_new_kernel)(void) = prog;
    jump_to_new_kernel();
}