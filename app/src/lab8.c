# include "user_lib.h"

void main(){
  int cnt = 0;
  char ct[20];
  if(fork() == 0) {
    fork();
    fork();
    while(cnt < 10) {
      //printf("pid: %d, sp: 0x%llx cnt: %d\n", getpid(), &cnt, cnt++); // address should be the same, but the cnt should be increased indepndently
      uart_write((char *) "pid: ", 5);
      int_to_str(getpid(), ct);
      uart_write(ct, str_len(ct));
      
      uart_write((char *) ", sp: ", 7);
      ptr_to_hex((void*)(&cnt), ct);
      uart_write(ct, str_len(ct));
      
      uart_write((char *) ", cnt: ", 8);
      int_to_str(cnt, ct);
      uart_write(ct, str_len(ct));
      uart_write((char *) "\n", 1);
      
      cnt++;
      delay(1000000);
    }
  } else {
    int* a = 0x0; // a non-mapped address.
    int_to_str(*a, ct);
    uart_write(ct, str_len(ct)); // trigger simple page fault.
    uart_write("\nShould not be printed\n", 23);
  }
}

