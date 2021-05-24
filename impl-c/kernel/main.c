#include "cfg.h"
#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "mm.h"
#include "mm/startup.h"
#include "proc.h"
#include "shell/shell.h"
#include "test.h"
#include "uart.h"

#define ANSI_GREEN(s) ("\033[0;32m" s "\033[0m")

#define MX_CMD_BFRSIZE 64
extern unsigned char __kernel_start, __kernel_end;

static void init_sys(char *name, void (*func)(void));
static void run_shell();
static void reserve_startup_area();

/**
 * Kernel main function
 * Power up the whole system
 */
void main() {
  uart_init();
  uart_println("uart initialized");

  init_sys("Init Startup allocator", startup_init);
  init_sys("Reserve memory area", reserve_startup_area);
  init_sys("Init Memory Allocator", KAllocManager_init);

  // KAllocManager_run_example();
  // KAllocManager_show_status();

  init_sys("Init Proc subsystem", proc_init);

#ifdef CFG_RUN_TEST
  run_tests();
#endif

  vfs_init();
  register_filesystem(&tmpfs);

  uart_println("finished, start busy waiting...");
  while (1) {
    ;
  }
  // test_tasks();
  // run_shell();
}
// =====

void init_sys(char *name, void (*func)(void)) {
  uart_printf("%s ...", name);
  func();
  uart_println("\r%s ... %s", name, ANSI_GREEN("success"));
}

void reserve_startup_area() {
  // Kernel
  startup_reserve((void *)0x0, 0x1000);      // spin table
  startup_reserve((void *)0x60000, 0x20000); // stack
  startup_reserve((void *)(&__kernel_start),
                  (&__kernel_end - &__kernel_start)); // kernel
  // startup_reserve((void *)(&kn_end), mem_size / PAGE_SIZE);    // buddy
  // System
  startup_reserve((void *)0x3f000000, 0x1000000); // MMIO
}

void run_shell() {
  struct Shell sh;
  char shell_buffer[MX_CMD_BFRSIZE + 1];
  shell_init(&sh, shell_buffer, MX_CMD_BFRSIZE);

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");
  uart_println(" input filename to see file content");
  while (1) {
    shell_show_prompt(&sh);
    shell_input_line(&sh);
    shell_process_command(&sh);
  }
}