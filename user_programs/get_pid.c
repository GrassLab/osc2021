#include "lib.h"
int main() {
  int i;
  i = getpid();
  for (int j = 0; j < 5; j++) {
    asm volatile("svc 0\n" :);
  }
  while (1) {
    ;
  }
}