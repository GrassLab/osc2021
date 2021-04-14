#include "shell.h"
#include "mmio.h"
#include "util.h"

#define GETS_BUFF_LEN 0xff
#ifdef _QEMU
#define KERNEL_ADDR 0x90000
#else
#define KERNEL_ADDR 0x80000
#endif // _QEMU

int help() {
  puts("Commands:\n\r");
  puts("\thelp:\tGet informations.\n\r");
  puts("\tload:\tLoad kernel file from uart.\n\r");
  puts("\treboot:\tReboot OS from SD Card.\n\r");
  return 0;
}

int reboot() {
  reset(1);
  return 0;
}

int load_kernel() {
  void *kernel_base = (void *)KERNEL_ADDR;
  void *kernel_curr = kernel_base;
  char c;
  // get file size (kernel image size)
  unsigned int file_size = 0;
  file_size += (unsigned int)uart_getc() << (8 * 3);
  file_size += (unsigned int)uart_getc() << (8 * 2);
  file_size += (unsigned int)uart_getc() << (8 * 1);
  file_size += (unsigned int)uart_getc() << (8 * 0);

  // read kernel image
  while (kernel_curr < kernel_base + file_size) {
    c = uart_getc();
    *(char *)kernel_curr = c;
    kernel_curr++;
  }

  // Jump to KERNEL_ADDR
  ((void (*)(void))kernel_base)();
  return 0;
}

int _clear() {
  puts("\n\r");
  return 0;
}

int _test() {
  puts("123456");
  uart_setc(12);
  return 0;
}

int cmd(const char *buff) {
  if (!strcmp(buff, "help")) {
    return help();
  } else if (!strcmp(buff, "reboot")) {
    return reboot();
  } else if (!strcmp(buff, "load")) {
    return load_kernel();
  } else if (!strcmp(buff, "")) {
    return 1;
  } else {
    puts(buff);
    puts(" : command not found!\n\r");
    return 1;
  }
  return 0;
}

void _welcome() {
  puts("bootloader...\n\r");
  puts("\n\r");
  return;
}

void shell() {
  _clear();
  _welcome();
  while (1) {
    int rtn = 0;
    puts("(help to get info.): ");
    char buff[GETS_BUFF_LEN];
    gets(buff);
    rtn = cmd(buff);
  }
}
