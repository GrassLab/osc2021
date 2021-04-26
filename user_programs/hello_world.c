#include "lib.h"
int main() {
  char *msg = "Hello, world!\n";
  uart_write(msg, 15);
  while (1) {
    ;
  }
}