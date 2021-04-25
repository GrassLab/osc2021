#include "syscall.h"
#include "uart.h"
void syscall_routing(int num, struct trap_frame *tf) {
  uart_println("syscall routing, num=%d", num);
  switch (num) {
  case SYS_GETPID:
    sys_getpid(tf);
    break;
  case SYS_UART_WRITE:
    sys_uart_write(tf);
    break;
  default:
    uart_println("syscall not implemented: %d", num);
    while (1) {
      ;
    }
    break;
  }
}