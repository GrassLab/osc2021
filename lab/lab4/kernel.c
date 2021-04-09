#include "kernel.h"

/*  do_funcs */
void do_reset(int tick) {         // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20;  // full reset
  *PM_WDOG = PM_PASSWORD | tick;  // number of watchdog tick
}
void do_help() {
  uart_puts("help: print available commands\r\n");
  uart_puts("reboot: restart device\r\n");
  uart_puts("ls: list file\r\n");
  uart_puts("cat: print file context\r\n");
  uart_puts("clear: clean screen\r\n");
}
void do_except(char *buff) {
  uart_puts("No command: ");
  uart_puts(buff);
  uart_puts("\r\n");
}
void do_ls() {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  while (!strcmp("TRAILER!!!", filename = now_addr + CPIO_SIZE)) {
    uart_puts(filename);
    uart_puts("\r\n");
    cpio_info(&cpio_header, &now_addr, &context);
  }
}
void do_cat(char *buff) {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  int k = 0;
  unsigned long long int context_size = 0;
  while (buff[k] != '\0') k++;
  do {
    filename = now_addr + CPIO_SIZE;
    context_size = cpio_info(&cpio_header, &now_addr, &context);
  } while (!strcmp("TRAILER!!!", filename) && !strcmp(&buff[k + 1], filename));
  /* check file exist */
  if (!strcmp(&buff[k + 1], filename)) {
    uart_puts("cat: ");
    uart_puts(&buff[k + 1]);
    uart_puts(": No such file or directory\r\n");
  }
  /* get cpio context size */
  for (int i = 0; i < context_size; i++) uart_send(context[i]);
  uart_puts("\r\n");
}
void do_clear() { uart_puts("\033c"); }
void do_run(char *buff) {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  int k = 0;
  unsigned long long int context_size = 0;
  buff = "run\0test.img\0";  //+++++++++++++++++++++++++++++++++++++++++
  while (buff[k] != '\0') k++;
  do {
    filename = now_addr + CPIO_SIZE;
    context_size = cpio_info(&cpio_header, &now_addr, &context);
  } while (!strcmp("TRAILER!!!", filename) && !strcmp(&buff[k + 1], filename));
  /* check file exist */
  if (!strcmp(&buff[k + 1], filename)) {
    uart_puts("run: ");
    uart_puts(&buff[k + 1]);
    uart_puts(": No such file or directory\r\n");
    return;
  }
  /* get cpio context size */
  print_h(_get_el());
  char *process_location = (char *)malloc(context_size);
  // print_h(process_location);
  for (int i = 0; i < context_size; i++) process_location[i] = context[i];
  _run_el0(process_location, process_location + context_size);
  // print_h(context_size);
  print_h(_get_el());
  // _branch(process_location);
  free(process_location);
}

void shell() {
  char buff[buff_size];
  /* say hello */
  uart_puts("\r\n++++++++++++++++++++++\r\n");
  uart_puts("+++ Hello Kernel!! +++\r\n");
  uart_puts("++++++++++++++++++++++\r\n");

  while (1) {
    uart_puts("$ ");
    get_cmd(buff);
    if (buff[0] == '\0')
      continue;
    else if (strcmp(buff, "help"))
      do_help();
    else if (strcmp(buff, "ls"))
      do_ls();
    else if (strcmp(buff, "cat"))
      do_cat(buff);
    else if (strcmp(buff, "reboot")) {
      do_reset(100);
      return;

    } else if (strcmp(buff, "clear"))
      do_clear();
    else if (strcmp(buff, "run"))
      do_run(buff);
    else if (strcmp(buff, "lab3")) {
      // buddy_test1();
      // buddy_test4();
      // dma_test1();
      // dma_test2();
    } else
      do_except(buff);
  }
}

void main() {
  uart_init();  // set up serial console
  buddy_init((char *)BUDDY_START);
  dma_init();
  do_clear();
  _core_timer_enable(2);
  shell();
}