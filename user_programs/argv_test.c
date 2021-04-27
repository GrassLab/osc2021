#include "lib.h"
int main() {
  int i;
  const char *target = "./hello_world.out";
  i = getpid();
  asm volatile("svc 0\n" :);
  exec(target, NULL);
  return 0;
}