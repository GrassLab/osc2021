#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");
  uart_println(" input filename to see file content");

  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}