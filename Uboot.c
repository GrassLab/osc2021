#include "uart.h"

void main()
{
    uart_init();

    int ptr = 0, size = 0;
    char buffer[50];
    char* kernel = (char*)0x80000;

    uart_gets(buffer, 50);
    while (buffer[0] != 'U') {
        uart_gets(buffer, 50);
        uart_puts(buffer);
        uart_puts("\n");
    }

    uart_puts("Need image size\n");

    uart_gets(buffer, 50);
    while (buffer[ptr] != '\0') {
        size *= 10;
        size += buffer[ptr] - '0';
        ptr++;
    }

    if (size == 1453) {
        uart_puts("Correct size\n");
    } else {
        uart_puts("Wrong size\n");
    }

    while (size--) {
        *kernel++ = uart_getc();
    }

    uart_puts("Sending end\n");

    asm volatile(
        "mov x30, 0x80000; ret");
}