#include "shell.h"
#include "uart.h"
int main() {
  uart_init();

  uart_puts("\r🐹🐔🐤🦊:  \\Hello/\n# ");
  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}