#include "shell.h"
#include "util.h"
#include "mmio.h"


void help() {
  puts("Commands:\n\r");
  puts("    help\n\r");
  puts("    hello\n\r");
  puts("    reboot\n\r");
  return;
}

void hello() {
  puts("Hello World!\n\r");
  return;
}

void reboot() { reset(1); }

void cmd(const char *buff) {
  if (!strcmp(buff, "help")) {
    help();
  } else if (!strcmp(buff, "hello")) {
    hello();
  } else if (!strcmp(buff, "reboot")) {
    reboot();
  } else if (!strcmp(buff, "")) {
    return;
  } else {
    puts(buff);
    puts(" : command not found!\n\r");
  }
  return;
}
