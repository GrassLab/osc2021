#include "alloc.h"
#include "mini_uart.h"
#include "shell.h"

void main() {
  uart_init();
  buddy_init();
  run_shell();
}
