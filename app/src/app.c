# include "user_lib.h"

const char *ca = "Hi\n";

void main(){
  uart_write(ca, str_len(ca));
  int *a = (int*)0x700;
  *a = 0;
  uart_write("Should not be printed\n", 22);
}
