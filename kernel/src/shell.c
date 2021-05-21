#include "shell.h"

#include "alloc.h"
#include "cpio.h"
#include "dtb.h"
#include "exception.h"
#include "mini_uart.h"
#include "printf.h"
#include "string.h"
#include "thread.h"
#include "timer.h"
#include "vfs.h"

void cmd_help() {
  printf("Command\t\tDescription\n");
  printf("---------------------------------------------\n");
  printf("help\t\tprint all available commands\n");
  printf("hello\t\tprint Hello World!\n");
  printf("reboot\t\treboot machine\n");
  printf("ls\t\tlist files in Cpio archive\n");
  printf("cat\t\tprint file content given pathname in Cpio archive\n");
  printf("dtb\t\tparse and print the flattened devicetree\n");
  printf("buddy\t\ttest buddy system\n");
  printf("dma\t\ttest dynamic memory allocator\n");
  printf("run\t\tload and run a user program in the initramfs\n");
  printf("puts\t\tasynchronous puts\n");
  printf("setTimeout [MESSAGE] [SECONDS]\t\tprints MESSAGE after SECONDS\n");
}

void cmd_hello() { printf("Hello World!\n"); }

void cmd_reboot(int tick) {       // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

void cmd_ls() { cpio_ls(); }

void cmd_cat(char *pathname) { cpio_cat(pathname); }

void cmd_dtb_print(int all) { dtb_print(all); }

void cmd_buddy_test() { buddy_test(); }

void cmd_dma_test() { dma_test(); }

void cmd_load_user_program(const char *program_name) {
  uint64_t spsr_el1 = 0x0;  // EL0t with interrupt enabled
  uint64_t target_addr = 0x30000000;
  uint64_t target_sp = 0x31000000;
  cpio_load_user_program(program_name, target_addr);
  core_timer_enable();
  asm volatile("msr spsr_el1, %0" : : "r"(spsr_el1));
  asm volatile("msr elr_el1, %0" : : "r"(target_addr));
  asm volatile("msr sp_el0, %0" : : "r"(target_sp));
  asm volatile("eret");
}

void cmd_set_timeout(char *args) {
  uint32_t duration = 0;
  for (int i = 0; args[i]; i++) {
    if (args[i] == ' ') {
      for (int j = i + 1; args[j]; j++) {
        duration = duration * 10 + (args[j] - '0');
      }
      args[i] = '\0';
      break;
    }
  }
  add_timer(timer_callback, args, duration);
}

void cmd_thread_test1() { thread_test1(); }

void cmd_thread_test2() { thread_test2(); }

void cmd_thread_vfs_test() { thread_vfs_test(); }

void clear_buffer() {
  buffer_pos = 0;
  for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
    buffer[i] = '\0';
  }
}

void receive_cmd() {
  while (1) {
    char c = uart_async_getc();
    if (c == '\0') continue;  // to avoid weird character
    if (c == '\n') {          // '\r' is replaced with '\n'
      printf("\n");
      buffer[buffer_pos] = '\0';
      break;
    }
    printf("%c", c);
    buffer[buffer_pos++] = c;
  }
}

void run_shell() {
  printf("************************************\n");
  printf("** Operating System Capstone 2021 **\n");
  printf("************************************\n");
  while (1) {
    enable_interrupt();  // need uart interrupt when go back to shell
    printf("%% ");
    clear_buffer();
    receive_cmd();
    if (strcmp(buffer, "help") == 0) {
      cmd_help();
    } else if (strcmp(buffer, "hello") == 0) {
      cmd_hello();
    } else if (strcmp(buffer, "reboot") == 0) {
      cmd_reboot(100);
      break;
    } else if (strcmp(buffer, "ls") == 0) {
      cmd_ls();
    } else if (strncmp(buffer, "cat", 3) == 0) {
      cmd_cat(&buffer[4]);
    } else if (strcmp(buffer, "dtb") == 0) {
      cmd_dtb_print(0);
    } else if (strcmp(buffer, "dtb all") == 0) {
      cmd_dtb_print(1);
    } else if (strcmp(buffer, "buddy") == 0) {
      cmd_buddy_test();
    } else if (strcmp(buffer, "dma") == 0) {
      cmd_dma_test();
    } else if (strncmp(buffer, "run", 3) == 0) {
      cmd_load_user_program(&buffer[4]);
    } else if (strcmp(buffer, "puts") == 0) {
      uart_async_puts("async puts\n");
    } else if (strncmp(buffer, "setTimeout", 10) == 0) {
      cmd_set_timeout(&buffer[11]);
    } else if (strcmp(buffer, "thread test 1") == 0) {
      cmd_thread_test1();
    } else if (strcmp(buffer, "thread test 2") == 0) {
      cmd_thread_test2();
    } else if (strcmp(buffer, "vfs") == 0) {
      cmd_thread_vfs_test();
    }
  }
}
