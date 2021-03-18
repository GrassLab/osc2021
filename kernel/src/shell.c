#include "shell.h"
#include "util.h"
#include "mmio.h"

#define GETS_BUFF_LEN 0xff


int help() {
  puts("Commands:\n\r");
  puts("    help\n\r");
  puts("    hello\n\r");
  puts("    reboot\n\r");
  return 0;
}

int hello() {
  puts("Hello World!\n\r");
  return 0;
}

int reboot() {
  reset(1);
  return 0;
}

int load_kernel() {
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
  } else if (!strcmp(buff, "hello")) {
    return hello();
  } else if (!strcmp(buff, "reboot")) {
    return reboot();
  } else if (!strcmp(buff, "load")) {
    return load_kernel();
  } else if (!strcmp(buff, "test")) {
    return _test();
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
  puts("OSC 2021...\n\r");
  return;
}

void shell() {
  _clear();
  _welcome();
  while (1) {
    int rtn = 0;
    puts("$ ");
    char buff[GETS_BUFF_LEN];
    gets(buff);
    rtn = cmd(buff);
  }
}
