#include "lib.h"

int main() {
  int a = open("hello", FILE_O_CREAT);
  char buf[200];
  int size = 0;
  size = read(a, buf, 200);
  if (size > 0) {
    printf("read data: %s\n", buf);
  }
  int ret = close(a);
  printf("Closed? %s\n", ret == 0 ? "yes" : "no");
  return 0;
}