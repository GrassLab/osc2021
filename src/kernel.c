#include "io.h"
#include "reset.h"
#include "util.h"

void shell() {
  char buf[256];
  gets_n(buf, 255);
  if (!strcmp(buf, "hello")) {
    puts("Hello World!\n");
  } else if (!strcmp(buf, "help")) {
    puts(
        "help   : help menu\n"
        "hello  : print hello\n"
        "reboot : reboot system\n");
  } else if (!strcmp(buf, "reboot")) {
    reset(5);
  } else if (strcmp(buf, "")) {
    puts("no such instruction \'");
    puts(buf);
    puts("\'\n");
  }
}

void kernel() {
  puts("Lab 2:\n");
  while (1) {
    shell();
  }
}