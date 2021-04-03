#include "cfg.h"
#include "mem.h"
#include "shell.h"
#include "test.h"
#include "uart.h"

int main() {
  uart_init();
  uart_println("uart initialized");

#ifdef CFG_RUN_TEST
  run_tests();
#endif

  // KAllocManager_init();
  // KAllocManager_show_status();

  // void *a[30];
  // for (int i = 0; i < 5; i++) {
  //   a[i] = kalloc(8192);
  //   uart_println("i:%d, a: %x", i, a[i]);
  // }

  // for (int i = 0; i < 5; i++) {
  //   kfree(a[i]);
  // }

  // for (int i = 0; i < 5; i++) {
  //   a[i] = kalloc(13);
  //   uart_println("i:%d, a: %x", i, a[i]);
  // };
  // if (a[0] == a[1]) {
  //   uart_println("nooo");
  // }
  // for (int i = 0; i < 5; i++) {
  //   kfree(a[i]);
  // }

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