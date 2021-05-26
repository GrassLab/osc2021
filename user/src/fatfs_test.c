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
      printf("Name: %s\n", name);
    }
  }

  return 0;
}