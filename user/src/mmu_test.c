#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main(int argc, char** argv) {
  printf("Program: %s, pid: %d\n", argv[0], getpid());

  int cnt = 0;
  if (fork() == 0) {
    fork();
    fork();
    while (cnt < 10) {
      // address should be the same, but the cnt should be increased
      // indepndently
      printf("pid: %d, sp: 0x%x cnt: %d\n", getpid(), &cnt, cnt++);
      delay(1000000);
    }
  } else {
    int* a = 0x1234;     // a non-mapped address.
    printf("%d\n", *a);  // trigger simple page fault.
    printf("Should not be printed\n");
  }
  return 0;
}