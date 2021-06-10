# include "user_lib.h"

void main(){
  char buf[30];
  int b = open("hello", O_RD);
  int a = open("world", O_RD);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_write(buf, sz+1);
  exit();
}
