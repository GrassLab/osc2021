#include "mini_uart.h"

void kernel_start(void){
    uart_init();
    uart_puts("Hello, world!\n");

    while(1){
        uart_puts("YOYOYO\n");
        uart_putc(uart_getc());
    }
}