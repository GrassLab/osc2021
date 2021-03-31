#include "shell.h"

#include "alloc.h"
#include "cpio.h"
#include "dtb.h"
#include "io.h"
#include "mini_uart.h"
#include "string.h"

void cmd_help() {
  print_s("Command\t\tDescription\n");
  print_s("---------------------------------------------\n");
  print_s("help\t\tprint all available commands\n");
  print_s("hello\t\tprint Hello World!\n");
  print_s("reboot\t\treboot machine\n");
  print_s("ls\t\tlist files in Cpio archive\n");
  print_s("cat\t\tprint file content given pathname in Cpio archive\n");
  print_s("dtb\t\tparse and print the flattened devicetree\n");
}

void cmd_hello() { print_s("Hello World!\n"); }

void cmd_reboot(int tick) {       // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

void cmd_ls() { cpio_ls(); }

void cmd_cat(char *pathname) { cpio_cat(pathname); }

void cmd_dtb_print(int all) { dtb_print(all); }

void cmd_buddy_test() { buddy_test(); }

void cmd_dma_test() { dma_test(); }

void clear_buffer() {
  buffer_pos = 0;
  for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
    buffer[i] = '\0';
  }
}

void receive_cmd() {
  while (1) {
    char c = uart_getc();
    if (c == '\0') continue;  // to avoid weird character
    if (c == '\n') {          // '\r' is replaced with '\n'
      print_s("\r\n");
      buffer[buffer_pos] = '\0';
      break;
    }
    print_c(c);
    buffer[buffer_pos++] = c;
  }
}

void run_shell() {
  print_s("************************************\n");
  print_s("** Operating System Capstone 2021 **\n");
  print_s("************************************\n");
  while (1) {
    print_s("% ");
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
    }
  }
}
