#include "alloc.h"
#include "mini_uart.h"
#include "printf.h"
#include "shell.h"
#include "thread.h"
#include "timer.h"
#include "vfs.h"
int a;
void main() {
  uart_init();
  buddy_init();
  timeout_event_init();
  thread_init();
  vfs_init();
  printf("0x%llx\n", &a);
  int b;
  printf("%p\n", &b);
  run_shell();
}
