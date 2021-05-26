#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main(int argc, char** argv) {
  printf("Program: %s, pid: %d\n", argv[0], getpid());

  mkdir("/fat");
  mount("fatfs", "/fat", "fatfs");

  int fd = open("/fat", 0);
  char name[100];
  int size;
  for (int i = 0;; ++i) {
    size = list(fd, name, i);
    // printf("size %d\n", size);
    if (size < 0) break;
    if (size > 0) {
      printf("Name: %s, size: %d\n", name, size);
    }
  }
  close(fd);

  printf("\nRead CONFIG.TXT\n");
  fd = open("/fat/CONFIG.TXT", 0);
  // printf("fd %d", fd);
  int sz;
  char buf[200];
  sz = read(fd, buf, 200);
  buf[sz] = '\0';
  printf("%s\n", buf);
  close(fd);

  fd = open("/fat/CONFIG.TXT", 0);
  printf("\nFirst write CONFIG.TXT\n");
  write(fd, "123456", 6);
  close(fd);

  fd = open("/fat/CONFIG.TXT", 0);
  sz = read(fd, buf, 200);
  buf[sz] = '\0';
  printf("%s\n", buf);

  fd = open("/fat/CONFIG.TXT", 0);
  printf("\nSecond write CONFIG.TXT\n");
  write(fd, "kernel", 6);
  close(fd);

  fd = open("/fat/CONFIG.TXT", 0);
  sz = read(fd, buf, 200);
  buf[sz] = '\0';
  printf("%s\n", buf);

  return 0;
}