#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" Boot Loader");
  uart_println("-------------------------------");

  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}