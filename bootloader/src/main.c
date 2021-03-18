#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" OSC 2021 Bootloader");
  uart_println(" # main is at: %x", &main);
  Shell sh;
  char buffer[MX_CMD_BFRSIZE + 1] = {0};
  initShell(&sh, (char *)buffer);
  while (1) {
    shellPrintPrompt(&sh);
    shellInputLine(&sh);
    shellProcessCommand(&sh);
  }
}