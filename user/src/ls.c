#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main(int argc, char** argv) {
  printf("Program: %s, pid: %d\n", argv[0], getpid());

  printf("Directory: %s\n", argv[1]);

  int fd = open(argv[1], 0);
  char name[100];
  int size;
  for (int i = 0;; ++i) {
    size = list(fd, name, i);
    if (size < 0) break;
    if (size > 0) {
      printf("Name: %s, size: %d\n", name, size);
    }
  }

  return 0;
}