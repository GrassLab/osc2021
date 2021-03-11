#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" OSC 2021 Bootloader");
  uart_println(" # main is at: %x", &main);

  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}