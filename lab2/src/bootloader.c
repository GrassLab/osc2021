#include "uart.h"
#include "utils.h"
#include "reboot.h"

void secondBootloader() {
    /*
    receive kernel from host machine
    */
    int size;
    char input;
    char kernel_size[10];
    uart_init();
    uart_puts("Waiting for kernel image...\n");

    // get ketnel size
    while (1) {
        input = uart_getc();
        if (!input) break;
        size = size * 10 + (input - '0');
    }

    itoa(size, kernel_size);
    uart_puts("Kernel code size is ");
    uart_puts(kernel_size);
    uart_puts(" bytes\n");

    // receive kernel
    volatile unsigned char *kernel;
    unsigned int addr = 0x00080000;
    kernel = (unsigned char *) addr;

    for (int i = 0; i < size; i++) {
        input = uart_getc();
        kernel[i] = input;
    }
    void (*jump)(void) = kernel;
    uart_puts("Jumping to kernel\n");
    jump();
}