#include "shell.h"

#include "mini_uart.h"
#include "string.h"

void cmd_help() {
  uart_puts("Command\t\tDescription\n");
  uart_puts("help\t\tprint all available commands\n");
  uart_puts("hello\t\tprint Hello World!\n");
}

void cmd_hello() { uart_puts("Hello World!\n"); }

void clear_buffer() {
  buffer_pos = 0;
  for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
    buffer[i] = '\0';
  }
}

void receive_cmd() {
  while (1) {
    char c = uart_getc();
    uart_send(c);
    if (c == '\n') {
      buffer[buffer_pos] = '\0';
      break;
    }
    buffer[buffer_pos++] = c;
  }
}

void run_shell() {
  while (1) {
    uart_puts("% ");
    clear_buffer();
    receive_cmd();
    if (strcmp(buffer, "help") == 0) cmd_help();
    if (strcmp(buffer, "hello") == 0) cmd_hello();
  }
}
