#include "alloc.h"
#include "exception.h"
#include "mini_uart.h"
#include "shell.h"
#include "thread.h"
#include "timer.h"
#include "printf.h"

void main() {
  uart_init();
  buddy_init();
  timeout_event_init();
  enable_interrupt();
  thread_init();
  run_shell();
}
