#include "mem.h"
#include "shell.h"
#include "uart.h"

int main() {
  uart_init();

  BuddyAllocater buddy;
  buddy_init(&buddy, Frames);

  int a = buddy_alloc(&buddy, 1);
  int b = buddy_alloc(&buddy, 2);
  int c = buddy_alloc(&buddy, 1);

  // int d = buddy_alloc(&buddy, 10);
  buddy_free(&buddy, a);
  buddy_free(&buddy, c);
  buddy_free(&buddy, b);
  buddy_dump(&buddy);

  uart_println("-------------------------------");
  uart_println(" Operating System Capstone 2021");
  uart_println("-------------------------------");
  uart_println(" input filename to see file content");

  while (1) {
    shellPrintPrompt();
    shellInputLine();
    shellProcessCommand();
  }
}