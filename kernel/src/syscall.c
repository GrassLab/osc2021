#include "syscall.h"

#include <stddef.h>
#include <stdint.h>

#include "mini_uart.h"
#include "thread.h"
#include "vfs.h"

void syscall_handler(uint32_t syscall_number, trap_frame_t *trap_frame) {
  switch (syscall_number) {
    case SYS_UART_READ:
      sys_uart_read(trap_frame);
      break;
    case SYS_UART_WRITE:
      sys_uart_write(trap_frame);
      break;
    case SYS_GETPID:
      sys_getpid(trap_frame);
      break;
    case SYS_FORK:
      sys_fork(trap_frame);
      break;
    case SYS_EXEC:
      sys_exec(trap_frame);
      break;
    case SYS_EXIT:
      sys_exit();
      break;
    case SYS_OPEN:
      sys_open(trap_frame);
      break;
    case SYS_CLOSE:
      sys_close(trap_frame);
      break;
    case SYS_WRITE:
      sys_write(trap_frame);
      break;
    case SYS_READ:
      sys_read(trap_frame);
      break;
    case SYS_LIST:
      sys_list(trap_frame);
      break;
  }
}

void sys_uart_read(trap_frame_t *trap_frame) {
  char *buf = (char *)trap_frame->x[0];
  uint32_t size = (uint32_t)trap_frame->x[1];
  size = uart_gets(buf, size);
  trap_frame->x[0] = size;
}

void sys_uart_write(trap_frame_t *trap_frame) {
  char *buf = (char *)trap_frame->x[0];
  uart_puts(buf);
  trap_frame->x[0] = trap_frame->x[1];
}

void sys_getpid(trap_frame_t *trap_frame) {
  uint32_t pid = get_current()->pid;
  trap_frame->x[0] = pid;
}

void sys_fork(trap_frame_t *trap_frame) {
  uint64_t sp = (uint64_t)trap_frame;
  fork(sp);
}

void sys_exec(trap_frame_t *trap_frame) {
  const char *program_name = (char *)trap_frame->x[0];
  const char **argv = (const char **)trap_frame->x[1];
  exec(program_name, argv);
}

void sys_exit() { exit(); }

void sys_open(trap_frame_t *trap_frame) {
  const char *pathname = (const char *)trap_frame->x[0];
  int flags = (int)trap_frame->x[1];
  struct file *file = vfs_open(pathname, flags);
  int fd = thread_get_fd(file);
  trap_frame->x[0] = fd;
}

void sys_close(trap_frame_t *trap_frame) {
  int fd = (int)trap_frame->x[0];
  struct file *file = thread_get_file(fd);
  int result = vfs_close(file);
  trap_frame->x[0] = result;
}

void sys_write(trap_frame_t *trap_frame) {
  int fd = (int)trap_frame->x[0];
  struct file *file = thread_get_file(fd);
  const void *buf = (const void *)trap_frame->x[1];
  size_t len = (size_t)trap_frame->x[2];
  size_t size = vfs_write(file, buf, len);
  trap_frame->x[0] = size;
}

void sys_read(trap_frame_t *trap_frame) {
  int fd = (int)trap_frame->x[0];
  struct file *file = thread_get_file(fd);
  void *buf = (void *)trap_frame->x[1];
  size_t len = (size_t)trap_frame->x[2];
  size_t size = vfs_read(file, buf, len);
  trap_frame->x[0] = size;
}

void sys_list(trap_frame_t *trap_frame) {
  int fd = (int)trap_frame->x[0];
  struct file *file = thread_get_file(fd);
  void *buf = (void *)trap_frame->x[1];
  int index = (int)trap_frame->x[2];
  int size = vfs_list(file, buf, index);
  trap_frame->x[0] = size;
}
