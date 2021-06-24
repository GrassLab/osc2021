#include "start.h"

void main() {
  int a = open("hello", O_CREAT);
  int b = open("world", O_CREAT);
  write(a, "Hello ", 6);
  write(b, "World!", 6);
  close(a);
  close(b);
  b = open("hello", 0);
  a = open("world", 0);
  int sz;
  char buf[100];
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  print(buf);
}