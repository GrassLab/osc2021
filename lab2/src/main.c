#include <uart.h>
#include "shell.h"
#include <string.h>
#include <mbox.h>

void main() {
  
  uart_init();
  //get_board_revision(); 
  //uart_hex(mbox[5]);
  shell();
}
