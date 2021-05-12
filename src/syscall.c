#include "syscall.h"

#include "exc.h"
#include "io.h"
#include "sched.h"

long sys_read(unsigned long fd, char *buf, unsigned long size) {
  disable_interrupt();
  unsigned long i = 0;
  while (i < size && !recv_buf_empty()) {
    buf[i++] = getc();
  }
  enable_interrupt();
  return i;
}
long sys_write(unsigned long fd, const char *buf, unsigned long size) {
  for (unsigned long i = 0; i < size; i++) {
    putc(buf[i]);
  }
  return size;
}