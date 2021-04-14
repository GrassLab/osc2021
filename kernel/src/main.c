#include "alloc.h"
#include "exception.h"
#include "mini_uart.h"
#include "shell.h"
#include "timer.h"

void main() {
  uart_init();
  buddy_init();
  timeout_event_init();
  enable_interrupt();
  run_shell();
}
