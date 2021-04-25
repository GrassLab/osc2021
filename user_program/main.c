#include "lib.h"
int main() {
  char *msg = "Hello, world!\n";
  uart_write(msg, 15);
  // for (int i = 0; i < 5; i++) {
  //   pid = getpid();
  // }
  while (1) {
    ;
  }
}