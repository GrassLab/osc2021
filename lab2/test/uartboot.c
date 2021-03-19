#include "uart.h"
#define printf uart_puts

void loadimg() {

    volatile unsigned char *prog;
    unsigned char code;
    unsigned int size;
    unsigned int addr;
    char s[20];

    printf("Waiting for a program to be sent from the UART ...\n");
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
    printf("raspi3 executes code at ");
    itoa(addr, s);
    printf(s);
    printf(", size = ");
    itoa(size, s);
    printf(s);
    printf(" bytes.\n");
    printf("-----------------------------------------------------------");
    printf("------------\n");

    void (*jump_to_new_kernel)(void) = prog;
    jump_to_new_kernel();
}