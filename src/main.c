#include "str_util.h"
#include "uart.h"
#include "reset.h"

void shell() {
  char buf[256];
  uart_puts("\r> ");
  uart_gets(buf, 255);
  if (!strcmp(buf, "hello")) {
    uart_puts("Hello World!\n");
  } else if (!strcmp(buf, "help")) {
    uart_puts(
        "help   : help menu\n"
        "hello  : print hello\n"
        "reboot : reboot system\n");
  } else if (!strcmp(buf, "reboot")) {
    reset(10);
  } else if (strcmp(buf, "")) {
    uart_puts("no such instruction\n");
  }
}

void main() {
  uart_init();
  uart_puts("Lab 1:\n");
  while (1) {
    shell();
  }
}