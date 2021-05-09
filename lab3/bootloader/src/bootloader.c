#include "uart.h"
#include "utils.h"
#include "reboot.h"

void secondBootloader() {
    /*
    receive kernel from host machine
    */
    int size = 0;
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
    kernel = (unsigned char *) 0x80000;

    for (int i = 0; i < size; i++) {
        input = uart_getc();
        kernel[i] = input;
    }
    for(int i = 0; i < 100000; i++) {
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
    char buf[20];
    char *help = "help";
    char *reboot = "reboot";
    
    while(1) {
        uart_puts("#");
        input(buf);
        uart_send('\r');

        if(strcmp(buf, help)) {
            uart_puts("help: print all available commands\n");
            uart_puts("reboot: reboot\n");
            uart_puts("loadimg: Load kernel\n");
        }
        else if(strcmp(buf, reboot)) {
            reset(1000);
        }
        else if (strcmp(buf, "loadimg")) {
            secondBootloader();
        }
        else {
            uart_puts("Error: ");
            uart_puts(buf);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}


