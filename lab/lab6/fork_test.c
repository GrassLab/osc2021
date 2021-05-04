#include "user.h"
#define BUFF_SIZE 64
void test();
void main(int argc, char **argv) {
  char buff[BUFF_SIZE];
  int cnt = 1, ret = 0;
  _uart_puts("Fork Test, pid ");
  getpid();
  _uart_puts("\n");
  if ((ret = fork()) == 0) {  // child
    _uart_puts("pid: ");
    getpid();
    _uart_puts(", cnt: ");
    _uart_puts(itoa(cnt, buff));
    _uart_puts(", ptr: ");
    print_hex((unsigned long long)&cnt);
    ++cnt;
    fork();
    _uart_puts("child pid: ");
    getpid();
    _uart_puts("\n");
    while (cnt < 5) {
      user_delay(100);
      _uart_puts("pid: ");
      getpid();
      _uart_puts(", cnt: ");
      _uart_puts(itoa(cnt, buff));
      _uart_puts(", ptr: ");
      print_hex((unsigned long long)&cnt);
      ++cnt;
    }
  } else {
    _uart_puts("parent here, pid: ");
    getpid();
    _uart_puts(", child ");
    _uart_puts(itoa(ret, buff));
    _uart_puts("\n");
  }
}