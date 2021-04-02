#include "mem.h"
#include "shell.h"
#include "uart.h"

int main() {
  uart_init();
  KAllocManager_init();
  // KAllocManager_show_status();

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");

  void *a[30];
  for (int i = 0; i < 5; i++) {
    a[i] = kalloc(8192);
    uart_println("i:%d, a: %x", i, a);
  }

  for (int i = 0; i < 5; i++) {
    kfree(a[i]);
  }

  for (int i = 0; i < 5; i++) {
    a[i] = kalloc(13);
    uart_println("i:%d, a: %x", i, a);
  };
  if (a[0] == a[1]) {
    uart_println("nooo");
  }
  for (int i = 0; i < 5; i++) {
    kfree(a[i]);
  }

  // KAllocManager_show_status();
  // int d = buddy_alloc(&buddy, 10);
  // kfree(a);
  // kfree(c);

  // kfree(b);
  // KAllocManager_show_status();
  uart_println(" input filename to see file content");
  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}