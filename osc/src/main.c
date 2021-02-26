#include "uart.h"

int main() {
  uart_init();

  uart_puts("\rMy Simple Shell:\n# ");

  while (1) {
    char c = uart_getc();

    if (c < 127) {
      uart_send(c); // show character user typed on uart terminal
      // printASCII(c);
    }
  }
}