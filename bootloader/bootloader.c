#include "uart.h"
#include "utils.h"
#include "string.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024

void help() {
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

void reboot() {
    uart_puts("Rebooting...\n");
    reset(100);
}

void loadimg() {

    // read the kernel size    
    unsigned char input[20];
    uart_read_line(input, 0);
    uart_send('\r');
    int kernel_size = atoi(input, 10);

    char *s;
    itoa(kernel_size, s);
    uart_puts("Kernel size is: ");
    uart_puts(s);
    uart_puts("\n");

    // read kernel image then save to 0x80000
    volatile unsigned char *new_address = (unsigned char *)0x80000;
    for(int i = 0; i < kernel_size; i++) {
        unsigned char c = uart_getc_boot();
        new_address[i] = c;
    }
    
    uart_puts("new kernel\n");

    for(int i = 0; i < 10000; i++) {
        asm volatile("nop");
    }
    void (*jump_to_new_kernel)(void) = new_address;
    jump_to_new_kernel();
}

void main() {
    uart_init();
    char *welcome = "waiting for loading kerenl...\n";
    uart_puts(welcome);

    while(1) {
        uart_puts("#");
        char input[20];
        uart_read_line(input, 1);
        uart_send('\r');

        if(!strcmp(input, "help")) {
            help();
        }
        else if(!strcmp(input, "reboot")) {
            reboot();
        }
        else if(!strcmp(input, "loadimg")) {
            loadimg();
        }
        else {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }
}
