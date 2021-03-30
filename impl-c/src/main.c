#include "mem.h"
#include "shell.h"
#include "uart.h"

int main() {
  uart_init();
  KAllocManager_init();
  // KAllocManager_show_status();

  void *a[30];
  for (int i = 0; i < 5; i++) {
    a[i] = kalloc(14);
    uart_println("i:%d, a: %x", i, a);
  }
  kfree(a[2]);
  kfree(a[3]);
  a[2] = kalloc(14);
  a[3] = kalloc(14);

  for (int i = 5; i < 10; i++) {
    a[i] = kalloc(14);
  }
  for (int i = 5; i < 10; i++) {
    kfree(a[i]);
  }
  // KAllocManager_show_status();
  // int d = buddy_alloc(&buddy, 10);
  // kfree(a);
  // kfree(c);

  // kfree(b);
  // KAllocManager_show_status();

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