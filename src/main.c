#include "uart.h"

void main() {
    uart_init();
    uart_putchar('y');
    uart_putchar('\n');
    while(1) {
        uart_putchar(uart_getchar());
    }
}
