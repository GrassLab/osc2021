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
  uart_puts("dtb: print device tree\r\n");
  uart_puts("install: print driver function match devices\r\n");
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
  do buff[i] = _uart_getc();
  while (buff[i++]);

  kernel_size = atoi(buff);
  uart_puts("Kernel Size: ");
  uart_puts(buff);
  uart_puts("\r\n");

  /* get kernel */
  for (i = 0; i < kernel_size; i++) {
    new_addr[i] = _uart_getc();
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

  /* get fdt header*/
  struct fdt_header *dts_header = *(struct fdt_header **)_save_dts;

  /* shift to struct offset*/
  char *fp = (char *)dts_header + big2little(dts_header->off_dt_struct);
  int device_len = strlen(buff);
  int deep = 1, len, FDT_NODE;
  char *name, *value, *type;
  while (deep) {
    deep = dtb_getNode(&fp, &name, &value, &len, &type, &FDT_NODE, dts_header,
                       deep);
    if (FDT_NODE == FDT_NODE_BEGIN) {
      if (device_len == 0)
        for (int i = 2; i < deep; i++) uart_send('>');
    } else if (FDT_NODE == FDT_PROP) {
      if (device_len == 0)
        for (int i = 0; i < deep; i++) uart_send(' ');
      if (find(buff, name) > -1) uart_puts("  ");
    }
    if (find(buff, name) > -1)
      dtb_printNode(name, value, len, type, FDT_NODE, deep);
  }
}

void do_probe(char *buff) {
  /* get compatible name */
  int k = 0;
  while (buff[k] != '\0') k++;
  buff = &buff[k + 1];

  /* get fdt header*/
  struct fdt_header *dts_header = *(struct fdt_header **)_save_dts;

  /* shift to struct offset*/
  char *fp = (char *)dts_header + big2little(dts_header->off_dt_struct);
  int deep = 1, len, FDT_NODE, bool = 0;
  char *name, *value, *type;
  while (deep) {
    deep = dtb_getNode(&fp, &name, &value, &len, &type, &FDT_NODE, dts_header,
                       deep);
    if (FDT_NODE == FDT_NODE_BEGIN || FDT_NODE == FDT_NODE_END)
      bool = 0;
    else if (FDT_NODE == FDT_PROP && strcmp("compatible", type)) {
      for (int i = 0; i < len; i++, value += strlen(value))
        if (find(buff, value) > -1) {
          dtb_printNode(name, value, len, type, FDT_NODE_BEGIN, deep);
          bool = 1;
          break;
        }
    }
    if (bool) dtb_printNode(name, value, len, type, FDT_NODE, deep);
  }
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
    else if (strcmp(buff, "dtb"))
      do_dtb(buff);
    else if (strcmp(buff, "install"))
      do_probe(buff);
    else
      do_except(buff);
  }
}
void main() {
  uart_init(0);  // set up serial console
  while (_uart_getc() == '\0')
    ;
  _uart_getc();
  shell();
}