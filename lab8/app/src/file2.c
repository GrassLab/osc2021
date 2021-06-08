# include "user_lib.h"

void main(){
  char buf[30];
  //uart_write((char *) "File Test", 16);
  //uart_write((char *) "\n", 1);
  int b = open("hello", O_RD);
  int a = open("world", O_RD);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_write(buf, sz+1);
  //printf("%s\n", buf);
  //uart_write((char *) "File exit", 16);
  //uart_write((char *) "\n", 1);
  exit();
}
