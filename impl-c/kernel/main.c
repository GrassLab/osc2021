#include "cfg.h"
#include "mem.h"
#include "mm/startup.h"
#include "shell.h"
#include "test.h"
#include "uart.h"

int main() {
  uart_init();
  uart_println("uart initialized");

#ifdef CFG_RUN_TEST
  run_tests();
#endif

  startup_init();

  // startup_reserve((void *)0x0, 0x1000);      // spin table
  // startup_reserve((void *)0x60000, 0x20000); // stack
  startup_reserve((void *)0xa0000, 0x8000);
  // startup_reserve((void *)(&kn_start), (&kn_end - &kn_start)); // kernel
  // startup_reserve((void *)(&kn_end), mem_size / PAGE_SIZE);    // buddy
  // system
  // startup_reserve((void *)0x3f000000, 0x1000000); // MMIO

  KAllocManager_init();
  KAllocManager_show_status();

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