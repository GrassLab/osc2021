# include "user_lib.h"

void main(){
  char buf[30];
  //uart_write((char *) "File Test", 16);
  //uart_write((char *) "\n", 1);
  int a = open("demo/hello", O_CREAT | O_WR);
  int b = open("demo/world", O_CREAT | O_WR);
  write(a, "Hello ", 6);
  write(b, "World!", 6);
  close(a);
  close(b);
  //uart_write((char *) "write done\n", 16);
  b = open("demo/hello", O_RD);
  a = open("demo/world", O_RD);
  int sz;
  sz = read(b, buf, 100);
  sz += read(a, buf + sz, 100);
  buf[sz] = '\0';
  uart_write(buf, sz+1);
  uart_write((char *) "\n", 1);
  //printf("%s\n", buf);
  //uart_write((char *) "File exit", 16);
  //uart_write((char *) "\n", 1);
  exit();
}
