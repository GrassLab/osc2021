#include "cfg.h"
#include "exec.h"
#include "mm.h"
#include "mm/startup.h"
#include "shell/shell.h"
#include "test.h"
#include "uart.h"

#define MX_CMD_BFRSIZE 64
extern unsigned char __kernel_start, __kernel_end;

void svc_test() {
  for (int i = 0; i < 3; i++) {
    asm volatile("svc 0 \n");
  }
}

int main() {
  uart_init();
  uart_println("uart initialized");

#ifdef CFG_RUN_TEST
  run_tests();
#endif

  // _exec_usr(&svc_test, (void *)0x60000, 0x3c0);

  startup_init();

  // Kernel
  startup_reserve((void *)0x0, 0x1000);      // spin table
  startup_reserve((void *)0x60000, 0x20000); // stack
  startup_reserve((void *)(&__kernel_start),
                  (&__kernel_end - &__kernel_start)); // kernel
  // startup_reserve((void *)(&kn_end), mem_size / PAGE_SIZE);    // buddy
  // System
  startup_reserve((void *)0x3f000000, 0x1000000); // MMIO

  uart_println("Initializing memory allocator...");
  KAllocManager_init();
  // KAllocManager_run_example();
  // KAllocManager_show_status();

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");
  uart_println(" input filename to see file content");

  struct Shell sh;
  char shell_buffer[MX_CMD_BFRSIZE + 1];
  shell_init(&sh, shell_buffer, MX_CMD_BFRSIZE);
  while (1) {
    shell_show_prompt(&sh);
    shell_input_line(&sh);
    shell_process_command(&sh);
  }
}