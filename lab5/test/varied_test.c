#include "varied_test.h"
#include <printf.h>
#include <uart.h>
#include <varied.h>

void test_varied_buddy() {
  void* chunk1;
  int size;
  size = 0x401;
  printf("test_varied_buddy....\n");
  
  chunk1 = varied_malloc(size);
  
  buddy_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  varied_free(chunk1);
}

void test_varied_dynamic() {
  void* chunk1;
  int size;
  size = 0x400;
  printf("test_varied_dynamic....\n");
  
  chunk1 = varied_malloc(size);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  varied_free(chunk1);

}

void test_varied_main() {
  char c;
  while(1) {
    printf("varied testing...\n");
    printf("options:\n");
    printf("[0]: test_varied_buddy\n");
    printf("[1]: test_varied_dynamic\n");
    printf("[e]: exit\n");

    printf("input option...");
    c = uart_getc();
    printf("\n");
    switch(c) {
    case 'e':
      return;
    case '0':
      test_varied_buddy();
      buddy_status();
      break;
    case '1':
      test_varied_dynamic();
      dynamic_status();
      break;
    default:
      printf("unknown command\n");
      break;
    }
  }
}