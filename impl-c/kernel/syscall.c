#include "syscall.h"
#include "proc/sched.h"
#include "uart.h"

#include "config.h"
#include "log.h"

#ifdef CFG_LOG_SYSCALL
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

#define log(name) log_println("[syscall] -> %s", #name)

void syscall_routing(int num, struct trap_frame *tf) {
  switch (num) {

  case SYS_GETPID: {
    log(SYS_GETPID);
    int pid = sys_getpid();
    tf->regs[0] = pid;
    break;
  }

  case SYS_UART_WRITE: {
    log(SYS_UART_WRITE);
    const char *s = (const char *)tf->regs[0];
    size_t size = (size_t)tf->regs[1];
    size_t written = sys_uart_write(s, size);
    tf->regs[0] = written;
    break;
  }

  case SYS_UART_READ: {
    log(SYS_UART_READ);
    char *buf = (char *)tf->regs[0];
    size_t size = tf->regs[1];
    size_t read = sys_uart_read(buf, size);
    tf->regs[0] = read;
    break;
  }

  case SYS_EXEC: {
    // we could call schedule at the beginning of syscall_routing (every
    // syscall) but we only call it here for better logging and also serve a
    // proof of concept that task schedule works
    task_schedule();

    log(SYS_EXEC);
    const char *name = (const char *)tf->regs[0];
    char *const *argv = (char *const *)(tf->regs[1]);
    int ret = sys_exec(name, argv);
    tf->regs[0] = ret;
    break;
  }

  case SYS_EXIT: {
    log(SYS_EXIT);
    sys_exit();
    break;
  }

  case SYS_FORK: {
    log(SYS_FORK);
    int child_id = sys_fork(tf);
    tf->regs[0] = child_id;
    break;
  }

  case SYS_OPEN: {
    log(SYS_OPEN);
    const char *pathname = (const char *)tf->regs[0];
    int flags = (int)tf->regs[1];
    int ret = sys_open(pathname, flags);
    tf->regs[0] = ret;
    break;
  }

  case SYS_CLOSE: {
    log(SYS_CLOSE);
    int fd = (int)tf->regs[0];
    int ret = sys_close(fd);
    tf->regs[0] = ret;
    break;
  }

  case SYS_WRITE: {
    log(SYS_WRITE);
    int fd = (int)tf->regs[0];
    const void *buf = (const void *)tf->regs[1];
    int count = (int)tf->regs[2];
    int ret = sys_write(fd, buf, count);
    tf->regs[0] = ret;
    break;
  }

  case SYS_READ: {
    log(SYS_READ);
    int fd = (int)tf->regs[0];
    void *buf = (void *)tf->regs[1];
    int count = (int)tf->regs[2];
    int ret = sys_read(fd, buf, count);
    tf->regs[0] = ret;
    break;
  }

  default: {
    uart_println("syscall not implemented: %d", num);
    while (1) {
      ;
    }
    break;
  }
  }
}