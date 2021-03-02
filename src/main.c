#include "mini_uart.h"

void main() {
  uart_init();

  // say hello
  uart_puts("Hello World!\n");

  // echo everything back
  while (1) {
    uart_send(uart_getc());
  }
}