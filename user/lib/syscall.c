#include "syscall.h"

unsigned long uart_read(char buf[], size_t size) {
  unsigned long ret;
  asm volatile("svc 0");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

unsigned long uart_write(const char buf[], size_t size) {
  unsigned long ret;
  asm volatile("svc 1");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int getpid() {
  int ret;
  asm volatile("svc 2");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

unsigned long fork() {
  unsigned long ret;
  asm volatile("svc 3");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int exec(const char *name, char *const argv[]) {
  int ret;
  asm volatile("svc 4");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

void exit() {
  asm volatile("svc 5");
}

int open(const char *pathname, int flags) {
  int ret;
  asm volatile("svc 6");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int close(int fd) {
  int ret;
  asm volatile("svc 7");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int write(int fd, const void *buf, int count) {
  int ret;
  asm volatile("svc 8");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int read(int fd, void *buf, int count) {
  int ret;
  asm volatile("svc 9");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int list(int fd, void *buf, int index) {
  int ret;
  asm volatile("svc 10");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int mkdir(const char *pathname) {
  int ret;
  asm volatile("svc 11");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int chdir(const char *pathname) {
  int ret;
  asm volatile("svc 12");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int mount(const char *device, const char *mountpoint, const char *filesystem) {
  int ret;
  asm volatile("svc 13");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}

int umount(const char *mountpoint) {
  int ret;
  asm volatile("svc 14");
  asm volatile("mov %0, x0" : "=r"(ret));
  return ret;
}
