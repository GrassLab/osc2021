#include "syscall.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_SYSCALL
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

#define log(name) log_println("[syscall] -> %s", #name)

void syscall_routing(int num, struct trap_frame *tf) {
  switch (num) {
  case SYS_GETPID:
    log(SYS_GETPID);
    sys_getpid(tf);
    break;
  case SYS_UART_WRITE:
    log(SYS_UART_WRITE);
    sys_uart_write(tf);
    break;
  case SYS_UART_READ:
    log(SYS_UART_READ);
    sys_uart_read(tf);
    break;
  case SYS_EXEC:
    log(SYS_EXEC);
    sys_exec(tf);
    break;
  case SYS_EXIT:
    log(SYS_EXIT);
    sys_exit(tf);
    break;
  case SYS_FORK:
    log(SYS_FORK);
    sys_fork(tf);
    break;
  default:
    uart_println("syscall not implemented: %d", num);
    while (1) {
      ;
    }
    break;
  }
}