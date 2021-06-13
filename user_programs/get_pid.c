#include "lib.h"
int main() {
  getpid();
  for (int j = 0; j < 5; j++) {
    asm volatile("svc 0\n" :);
  }
  return 0;
}