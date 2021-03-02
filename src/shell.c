#include "shell.h"

#include "mini_uart.h"
#include "string.h"

void cmd_help() {
  uart_puts("Command\t\tDescription\n");
  uart_puts("help\t\tprint all available commands\n");
  uart_puts("hello\t\tprint Hello World!\n");
  uart_puts("reboot\t\treboot machine\n");
}

void cmd_hello() { uart_puts("Hello World!\n"); }

void cmd_reboot(int tick) {       // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
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
    if (c == '\n') {  // '\r' is replaced with '\n'
      uart_puts("\r\n");
      buffer[buffer_pos] = '\0';
      break;
    }
    uart_send(c);
    buffer[buffer_pos++] = c;
  }
}

void run_shell() {
  uart_puts("************************************\n");
  uart_puts("** Operating System Capstone 2021 **\n");
  uart_puts("************************************\n");
  while (1) {
    uart_puts("% ");
    clear_buffer();
    receive_cmd();
    if (strcmp(buffer, "help") == 0) cmd_help();
    if (strcmp(buffer, "hello") == 0) cmd_hello();
    if (strcmp(buffer, "reboot") == 0) {
      cmd_reboot(100);
      break;
    }
  }
}
