#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main() {
  printf("Fork Test, pid %d\n", getpid());

  int cnt = 1;
  int ret = 0;
  if ((ret = fork()) == 0) {  // child
    printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
    ++cnt;
    fork();
    while (cnt < 5) {
      printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
      delay(1000000);
      ++cnt;
    }
  } else {
    printf("parent here, pid %d, child %d\n", getpid(), ret);
  }

  return 0;
}