# include "user_lib.h"

void main(){
  char ca[4];
  ca[0] = 'H';
  ca[1] = 'i';
  ca[2] = '\n';
  ca[3] = '\0';
  uart_write(ca, 4);
  int *a = (int*)0x700;
  *a = 0;
  uart_write("Should not be printed\n", 22);
}
