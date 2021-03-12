#include "bootloader.h"

#include "uart.h"
#include "utils.h"
void do_reset(int tick) {         // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}

void do_hello() { uart_puts("Hello World!\r\n"); }
void do_help() {
  uart_puts("help: print available commands\r\n");
  uart_puts("hello: print Hello World!\r\n");
  uart_puts("reboot: restart device\r\n");
}
void do_except(char *buff) {
  uart_puts("No command: ");
  uart_puts(buff);
  uart_puts("\r\n");
}
void do_jump(char *new_addr) {
  uart_puts("Plz start to send kernel!\r\n");
  char buff[buff_size];
  unsigned int kernel_size, i = 0;

  /* get kernel size */
  do buff[i] = uart_getc();
  while (buff[i++]);

  kernel_size = atoi(buff);
  uart_puts("Kernel Size: ");
  uart_puts(buff);
  uart_puts("\r\n");

  /* get kernel */
  for (i = 0; i < kernel_size; i++) {
    new_addr[i] = uart_getc();
    uart_puts("\r");
    uart_puts(itoa(i + 1, buff));
    uart_puts(" / ");
    uart_puts(itoa(kernel_size, buff));
  }
  uart_puts("\r\nload finish\r\n");
  _branch((unsigned long int *)new_addr);  // jump to kernel
}

void do_load() {
  /* copy this process */
  char *now = _start, *end = _end, *new = _start + SHIFT_ADDR;
  for (; now <= end; now++, new ++) *new = *now;
  uart_puts("copy finish\r\n");

  /* get and jump copy func address */
  void (*func_ptr)() = do_jump;
  void (*func_call)(char *) = (void (*)(char *))((unsigned long int)func_ptr +
                                                 (unsigned long int)SHIFT_ADDR);
  func_call((char *)KERNEL_ADDR);
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
    } else if (strcmp(buff, "load"))
      do_load();
    else
      do_except(buff);
  }
}
void main() {
  uart_init();  // set up serial console
  while (uart_getc() == '\0')
    ;
  uart_getc();
  shell();
}