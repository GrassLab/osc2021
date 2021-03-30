#include "mmio.h"
#include "util.h"
#include "shell.h"


void kernel() {
  uart_init();
  shell();
  return;
}
