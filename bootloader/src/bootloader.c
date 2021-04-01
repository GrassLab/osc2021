#include "shell.h"

void bootloader() {
  uart_init();
  shell();
}
