#include <uart.h>
#include "shell.h"
#include <string.h>
#include <mbox.h>
#include <types.h>

void main() {
  
  uart_init();

  /*extern void *_bss_begin;
  extern void *_bss_end;
  uart_hex((size_t)&_bss_begin);
  uart_puts("\n");
  uart_hex((size_t)&_bss_end);*/
  //get_board_revision(); 
  //uart_hex(mbox[5]);
  shell();
}
