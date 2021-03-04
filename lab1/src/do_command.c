#include "include/do_command.h"
void do_command(char* command) {
  if(strncmp(command, "help", 5) == 0) {
        uart_puts("help: print all available commands\n");
        uart_puts("hello: print Hello World!\n");
        uart_puts("reboot: \n");
  }
  else if(strncmp(command, "hello", 6) == 0) {
    uart_puts("Hello World!\n");
  }
  else if(strncmp(command, "reboot", 7) == 0) {
    reset(100);
  }
}

