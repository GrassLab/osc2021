#include "shell.h"

#include "io.h"
#include "mini_uart.h"
#include "string.h"

void cmd_help() {
  print_s("Command\t\tDescription\n");
  print_s("---------------------------------------------\n");
  print_s("help\t\tprint all available commands\n");
  print_s("hello\t\tprint Hello World!\n");
  print_s("reboot\t\treboot machine\n");
  print_s("loadimg\t\tload image through UART\n");
}

void cmd_hello() { print_s("Hello World!\n"); }

void cmd_reboot(int tick) {       // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

void cmd_loadimg() {
  print_s("Please send the image with UART.\r\n");

  // read image
  int img_size = read_i();
  char *base_addr = (char *)(0x80000);
  for (int i = 0; i < img_size; i++) {
    *(base_addr + i) = read_b();
  }
  print_s("read done\n");

  // jump to kernel
  ((void (*)())(long long int)base_addr)();
}

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
  print_s("**************************\n");
  print_s("** 3rd stage bootloader **\n");
  print_s("**************************\n");
  while (1) {
    print_s("% ");
    clear_buffer();
    receive_cmd();
    if (strcmp(buffer, "help") == 0) cmd_help();
    if (strcmp(buffer, "hello") == 0) cmd_hello();
    if (strcmp(buffer, "reboot") == 0) {
      cmd_reboot(100);
      break;
    }
    if (strcmp(buffer, "loadimg") == 0) cmd_loadimg();
  }
}
