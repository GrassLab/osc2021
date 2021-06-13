# include "user_lib.h"

void main(){
  int cnt = 1;
  int ret = 0;
  char ct[20];
  uart_write((char *) "pid: ", 5);
  int_to_str(getpid(), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", cnt: ", 7);
  int_to_str(cnt, ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) ", &cnt: ", 8);
  ptr_to_hex((void *)(&cnt), ct);
  uart_write(ct, str_len(ct));
  uart_write((char *) "\n", 1);
  if ((ret = fork()) == 0){
    uart_write((char *) "pid: ", 5);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", cnt: ", 7);
    int_to_str(cnt, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", &cnt: ", 8);
    ptr_to_hex((void*)(&cnt), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    cnt++;
    fork();
    while(cnt < 5){
      uart_write((char *) "pid: ", 5);
      int_to_str(getpid(), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", cnt: ", 7);
      int_to_str(cnt, ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) ", &cnt: ", 8);
      ptr_to_hex((void *)(&cnt), ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) "\n", 1);
      delay(1000000);
      cnt++;
    }
  }
  else{
    uart_write((char *) "parent here, pid ", 17);
    int_to_str(getpid(), ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) ", child ", 8);
    int_to_str(ret, ct);
    uart_write(ct, str_len(ct));
    uart_write((char *) "\n", 1);
    delay(10000000);
  }
  exit();
}
