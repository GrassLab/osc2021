#include "cpio.h"
#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");

#define RAMFS_ADDR 0x8000000
  CpioSummaryInfo info;
  cpioInfo((void *)RAMFS_ADDR, &info);
  uart_println("cpio: total files: %d", info.numFiles);

  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}