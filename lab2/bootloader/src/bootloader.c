#include "uart.h"
#include "utils.h"
#include "reboot.h"

void secondBootloader() {
    /*
    receive kernel from host machine
    */
    int size;
    volatile char input;
    char kernel_size[10];
    char flag[5];

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
    //unsigned int addr = 0x00080000;
    kernel = (unsigned char *) 0x80000;

    for (int i = 0; i < size; i++) {
        //uart_puts("1 ");
        input = uart_getc();
        //input = 'c';
        //uart_puts("2 ");
        //for (int k=0;k<100;k++) asm volatile("nop");
        kernel[i] = input;
        //itoa(i, flag);
        //uart_puts(flag);
        //uart_puts("\n");
    }
    for(int i = 0; i < 10000; i++) {
        asm volatile("nop");
    }
    void (*jump)(void) = kernel;
    uart_puts("Jumping to kernel\n");
    jump();
}


void main() {
    uart_init();
    char *helloworld = "\n\
 _                     _ _               ________  ________ \n\
| |                   | (_)             |_   _|  \\/  |  __ \\\n\
| |     ___   __ _  __| |_ _ __   __ _    | | | .  . | |  \\/\n\
| |    / _ \\ / _` |/ _` | | '_ \\ / _` |   | | | |\\/| | | __ \n\
| |___| (_) | (_| | (_| | | | | | (_| |  _| |_| |  | | |_\\ \\\n\
\\_____/\\___/ \\__,_|\\__,_|_|_| |_|\\__, |  \\___/\\_|  |_/\\____/\n\
                                  __/ |                     \n\
                                 |___/                      \n\
    \n";
    uart_puts(helloworld);
    secondBootloader();
}


