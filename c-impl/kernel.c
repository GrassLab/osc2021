#include "mmio.h"
#include "util.h"
#include "shell.h"

#define GETS_BUFF_LEN 0xff


void kernel() {
  uart_init();
  uart_setc('H');
  uart_setc('i');
  uart_setc('\n');
  while (1) {
    puts("$ ");
    char buff[GETS_BUFF_LEN];
    gets(buff);
    cmd(buff);
  }
  return;
}
