#include "shell.h"
#include "mmio.h"
#include "util.h"

#define GETS_BUFF_LEN 0xff

int help() {
  putln("Commands:");
  putln("\thelp:\tGet informations.");
  putln("\thello:\tSay hello to OS.");
  putln("\treboot:\tReboot OS from SD Card.");
  return 0;
}

int hello() {
  putln("Hello World!");
  return 0;
}

int reboot() {
  reset(1);
  return 0;
}

int load_kernel() { return 0; }

int _clear() {
  putln("");
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
    putln(" : command not found!");
    return 1;
  }
  return 0;
}

void _welcome() {
  putln("OSC 2021...");
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
