#include "bootloader.h"

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

/* dts block */

void do_dtb(char *buff) {
  /* get device name*/
  int k = 0;
  while (buff[k] != '\0') k++;
  buff = &buff[k + 1];
  /* test block*/
  struct fdt_header *dts_header = *(struct fdt_header **)_save_dts;
  /* shift to struct offset*/
  char *fp = (char *)dts_header + big2little(dts_header->off_dt_struct),
       name[buff_size];
  int deep = -1, device_len = strlen(buff);
  while (1) {
    int FDT_NODE = big2little(get_int32((unsigned int **)&fp));

    if (FDT_NODE == 0x00000001) {
      char c;
      int i;
      for (i = 1; (c = get_char8(&fp)) != '\0'; i++) name[i - 1] = c;
      name[i - 1] = '\0';
      fp += align(i, 4);

      if ((device_len == 0) | (find(buff, name) > -1)) {
        if (!device_len)
          for (int i = 0; i < deep; i++) uart_send('>');
        uart_puts("name: ");
        uart_puts(name);
        uart_puts("\r\n");
      }
      deep++;
    } else if (FDT_NODE == 0x00000002)
      deep--;
    else if (FDT_NODE == 0x00000003) {
      /* FDT_PROP struct */
      int len = big2little(get_int32((unsigned int **)&fp));
      int nameoff = big2little(get_int32((unsigned int **)&fp));
      if (device_len == 0) {
        for (int i = 0; i < deep; i++) uart_send(' ');
      }
      if (find(buff, name) > -1) {
        uart_puts("  ");
        dtb_valueTypePrint((char *)dts_header +
                               big2little(dts_header->off_dt_strings) + nameoff,
                           &fp, len);
      } else
        fp += len;
      fp += align(len, 4);
    } else if (FDT_NODE == 0x00000004)
      continue;
    else if (FDT_NODE == 0x00000009)
      break;
  }
}

/* dts block */

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
    else if (strcmp(buff, "dtb"))
      do_dtb(buff);
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