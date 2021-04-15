#include "shell.h"
#include "cpio.h"
#include "mmio.h"
#include "util.h"

#define GETS_BUFF_LEN 0xff

int cmd_help() {
  putln("Commands:");
  putln("\thelp:\tGet informations.");
  putln("\thello:\tSay hello to OS.");
  putln("\treboot:\tReboot OS from SD Card.");
  putln("\tls:\tList all pathname in CPIO.");
  putln("\tcat:\tShow file content. You should enter file path later.");
  return 0;
}

int cmd_hello() {
  putln("Hello World!");
  return 0;
}

int cmd_reboot() {
  reset(1);
  return 0;
}

int _clear() {
  putln("");
  return 0;
}

int _test() {
  puts("123456");
  uart_setc(12);
  return 0;
}

int cmd_ls() {
  initramfs_ls();
  return 0;
}

int cmd_cat() {
  puts("File to show: ");
  char pathname[GETS_BUFF_LEN];
  gets(pathname);
  initramfs_cat(pathname);
  return 0;
}

int cmd(const char *buff) {
  if (!strcmp(buff, "help")) {
    return cmd_help();
  } else if (!strcmp(buff, "hello")) {
    return cmd_hello();
  } else if (!strcmp(buff, "reboot")) {
    return cmd_reboot();
  } else if (!strcmp(buff, "ls")) {
    return cmd_ls();
  } else if (!strcmp(buff, "cat")) {
    return cmd_cat();
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
