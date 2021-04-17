#include "mmio.h"
#include "shell.h"
#include "util.h"

void kernel() {
  uart_init();
  shell();
  return;
}
