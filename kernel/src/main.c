#include "alloc.h"
#include "mini_uart.h"
#include "mmu_values.h"
#include "printf.h"
#include "shell.h"
#include "thread.h"
#include "timer.h"
#include "vfs.h"

void main() {
  uart_init();
  buddy_init();
  timeout_event_init();
  thread_init();
  vfs_init();
  run_shell();
}
