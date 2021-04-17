#include "shell.h"
#include "mmio.h"


void bootloader() {
  uart_init();
  shell();
}
