#include "printf.h"
#include "sys_call.h"

int main(void) {
  char buf[] = "pid = 0 (inside)\r\n";
  int cnt = 0;
  if(fork() == 0) {
    fork();
    fork();
    buf[6] += getpid();
    uart_write(buf, 18);
    while(cnt < 10) {
      printf("pid: %d, sp: 0x%llx cnt: %d\r\n", getpid(), &cnt, cnt++); // address should be the same, but the cnt should be increased indepndently
      delay(1000000);
    }
  } else {
    int* a = 0x0; // a non-mapped address.
    printf("%d\n", *a); // trigger simple page fault.
    printf("Should not be printed\r\n");
  }
}
