#include "uart.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

// https://sourceware.org/git/?p=glibc.git;a=blob_plain;f=string/strcmp.c
int strcmp(const char *p1, const char *p2) {
  const unsigned char *s1 = (const unsigned char *)p1;
  const unsigned char *s2 = (const unsigned char *)p2;
  unsigned char c1, c2;
  do {
    c1 = (unsigned char)*s1++;
    c2 = (unsigned char)*s2++;
    if (c1 == '\0') return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

void reboot() {                     // reboot after watchdog timer expire
  *PM_RSTC = (PM_PASSWORD | 0x20);  // full reset
  *PM_WDOG = (PM_PASSWORD | 0x10);  // number of watchdog tick
}

void main() {
  uart_init();
  uart_puts("\n**** welcome ****\n");
  while (1) {
    uart_puts("> ");
    char input[100];
    uart_gets(input, 100);
    if (strcmp(input, "help") == 0) {
      uart_puts("Available Commands:\n");
      uart_puts("help      print all available commands\n");
      uart_puts("hello     print Hello World!\n");
      uart_puts("reboot    reset raspberry pi\n");
    } else if (strcmp(input, "hello") == 0) {
      uart_puts("Hello World!\n");
    } else if (strcmp(input, "reboot") == 0) {
      reboot();
    } else if (input[0] == '\0') {
      continue;
    } else {
      uart_puts("command not found\n");
    }
  }
}
