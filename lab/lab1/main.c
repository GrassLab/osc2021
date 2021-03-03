#include "./uart_gpio/uart.h"
#define buff_size 64
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))

void do_reset(int tick) {         // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

int strcmp(char *array_1, char *array_2) {
  int i;
  for (i = 0; array_1[i] != '\0'; i++)
    if (array_1[i] != array_2[i]) return 0;
  if (array_2[i] != '\0') return 0;
  return 1;
}
void mem_reset(char *buff, int size) {
  for (int i = 0; i < size; i++) buff[i] = '\0';
}

void do_hello() { uart_puts("Hello World!\r\n"); }
void do_help() {
  uart_puts("help: print available commands\r\n");
  uart_puts("hello: print Hello World!\r\n");
  uart_puts("reboot: restart OS\b\r\n");
}
void do_except(char *buff) {
  uart_puts("No command: ");
  uart_puts(buff);
  uart_puts("\r\n");
}
void clear_line() { uart_puts("\r                    "); }
void get_cmd(char *buff) {
  char c;
  mem_reset(buff, buff_size);
  for (int i = 0; (c = uart_getc()); i++) {
    if (c == '\r') {
      uart_puts("\r\n");
      break;
    }
    /* backspace handler*/
    else if (c == '\b') {
      buff[--i] = 0;
      clear_line();
      uart_puts("\r$ ");
      uart_puts(buff);
      if (i-- < 0) i = -1;
      continue;
    }
    buff[i] = c;
    uart_send(c);
  }
}

void shell() {
  char buff[buff_size];
  /* say hello */
  uart_puts("++++++++++++++++++++\r\n");
  uart_puts("+++ Hello Shell! +++\r\n");
  uart_puts("++++++++++++++++++++\r\n");

  while (1) {
    uart_puts("$ ");
    get_cmd(buff);
    if (strcmp(buff, "hello"))
      do_hello();
    else if (strcmp(buff, "help"))
      do_help();
    else if (strcmp(buff, "reboot"))
      do_reset(10);
    else
      do_except(buff);
  }
}
void main() {
  uart_init();  // set up serial console
  while (uart_getc() == '\0')
    ;

  shell();
}