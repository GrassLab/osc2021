#include "include/uart.h"
#include "include/do_command.h"
#include "include/string.h"
void main() {
  uart_init();
  
  uart_puts("Hello World!\n");
  uart_puts("% ");
  
  char command[256];
  memset(command, '\0');
  int i = 0;
  while(1) {
    char c = uart_getc();
    if(c != '\n') {
      command[i++] = c; 
      uart_send(c);
    }
    else {
      command[i] = '\0';
      if(i > 0) {
	uart_puts("\n");
	do_command(command);
      }
      uart_puts("% ");
      i = 0;
      memset(command, '\0');
    }
  }
}

