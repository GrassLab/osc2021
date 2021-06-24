#include "syscall.h"

#include "exc.h"
#include "io.h"
#include "sched.h"
#include "vfs.h"

long sys_read(unsigned long fd, char *buf, unsigned long size) {
  if (fd == 0) {
    disable_interrupt();
    unsigned long i = 0;
    while (i < size && !recv_buf_empty()) {
      buf[i++] = getc();
    }
    enable_interrupt();
    return i;
  } else {
    return vfs_read(fd, buf, size);
  }
}
long sys_write(unsigned long fd, const char *buf, unsigned long size) {
  if (fd == 1 || fd == 2) {
    for (unsigned long i = 0; i < size; i++) {
      putc(buf[i]);
    }
    return size;
  } else {
    return vfs_write(fd, buf, size);
  }
}