#include "mem.h"
#include "shell.h"
#include "uart.h"

int main() {
  uart_init();
  KAllocManager_init();
  KAllocManager_show_status();
  void *a = kalloc(1);

  void *b = kalloc(2);

  void *c = kalloc(1);
  uart_println("c: %x", c);
  // KAllocManager_show_status();
  // int d = buddy_alloc(&buddy, 10);
  kfree(a);
  kfree(c);

  kfree(b);
  KAllocManager_show_status();

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