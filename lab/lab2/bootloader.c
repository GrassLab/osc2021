#include "bootloader.h"

#include "uart.h"
#include "utils.h"
void mem_reset(char *buff, int size) {
  for (int i = 0; i < size; i++) buff[i] = '\0';
}

void get_cmd(char *buff) {
  char c;
  mem_reset(buff, buff_size);
  for (int i = 0; (c = uart_getc()); i++) {
    if (c == '\r') {
      uart_puts("\r\n");
      break;
    }
    /* backspace handler */
    else if (c == '\b') {
      buff[--i] = 0;
      if (i >= 0) uart_puts("\b \b");
      if (i-- < 0) i = -1;
      continue;
    }
    buff[i] = c;
    uart_send(c);
  }
}

/*  do_funcs */
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
void do_jump(char *new_address) { _branch((unsigned long int *)new_address); }
void do_load() {
  /* copy this process*/
  char *now = _start, *end = _end, *new = (char *)(_start + MOVE_BYTES);
  for (; now < end; now++, new ++) *new = *now;
  uart_puts("copy finish\n");

  /* get _branch address */
  // void (*func_ptr)() = _moveTo;
  void (*func_ptr)() = do_jump;
  func_ptr = func_ptr + MOVE_BYTES;
  // func_ptr(func_ptr);

  /* jump to new process*/
  // char buff[64];
  // itoa(_start, buff);
  // uart_puts(itoa(_start, buff));
  // uart_puts("\n");
  // uart_puts(itoa(func_ptr, buff));
  // uart_puts("\n");
  // uart_puts(itoa(_end, buff));
  // uart_puts("\n");
  // uart_puts("\n");

  // uart_puts(itoa((char *)(_start + MOVE_BYTES), buff));
  // uart_puts("\n");
  // unsigned long int fff = func_ptr;
  // func_ptr = func_ptr + MOVE_BYTES;
  // uart_puts(itoa(func_ptr, buff));
  // uart_puts("\n");
  // uart_puts(itoa(new, buff));
  // uart_puts("\n");
  // uart_puts("\n");

  // uart_puts(itoa((char *)(_start + MOVE_BYTES) - _start, buff));
  // uart_puts("\n");
  // uart_puts(itoa(func_ptr - fff, buff));
  // uart_puts("\n");
  // uart_puts(itoa(new - _end, buff));
  // uart_puts("\n");
  // uart_puts("\n");

  // _moveTo((unsigned long int *)func_ptr, (unsigned long int)MOVE_BYTES);
}

void shell() {
  char buff[buff_size];
  /* say hello */
  uart_puts("+++++++++++++++++++++\r\n");
  uart_puts("+++ 3rd Bootload! +++\r\n");
  uart_puts("+++++++++++++++++++++\r\n");

  while (1) {
    uart_puts("$ ");
    get_cmd(buff);
    if (buff[0] == '\0')
      continue;
    else if (strcmp(buff, "hello"))
      do_hello();
    else if (strcmp(buff, "help"))
      do_help();
    else if (strcmp(buff, "reboot")) {
      do_reset(100);
      return;
    } else if (strcmp(buff, "load")) {
      do_load();
      uart_puts("DDDDDD\n");
    } else
      do_except(buff);
  }
}
void main() {
  uart_init();  // set up serial console
  // while (uart_getc() == '\0')
  //   ;
  // uart_getc();
  shell();
}