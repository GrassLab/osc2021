#include "user.h"

void main(int argc, char** argv) {
  char buf[1024];
  int a = open("hello2", O_CREAT);
  int b = open("world2", O_CREAT);
  write(a, "Hello ", 6);
  write(b, "World!", 6);
  close(a);
  close(b);
  b = open("hello2", 0);
  a = open("world2", 0);
  int sz;
  if (b > 0) sz = read(b, buf, 100);
  if (a > 0) sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  _uart_puts(buf);
  _uart_puts("\n");
}