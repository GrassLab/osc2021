#include "shell.h"
#include "uart.h"
int main() {
  uart_init();

  uart_puts("\rMy Simple Shell:\n# ");
  while (1) {
    uart_puts("\r>");
    shellInputLine();
    shellProcessCommand();
  }
}