#include "buddy_test.h"
#include <printf.h>
#include <uart.h>
#include <buddy.h>

void test_buddy_free_merged() {
  void* chunk1, *chunk2;
  int size;
  size = 0x40000;
  printf("test_buddy_free_merged\n");
  
  chunk1 = buddy_malloc(size);
  chunk2 = buddy_malloc(size);
  
  buddy_status();
  buddy_free(chunk2);
  
  buddy_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");

  buddy_free(chunk1);  
}

void test_buddy_malloc_with_merged() {
  void* chunk1, *chunk2;
  int size;
  size = 0x40000;
  printf("test_buddy_malloc_with_merged\n");
  
  chunk1 = buddy_malloc(size);
  chunk2 = buddy_malloc(size);
  
  buddy_status(); 
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  buddy_free(chunk1);
  
  buddy_status(); 
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");

  buddy_free(chunk2);
}

void test_buddy_main() {
  char c;
  while(1) {
    printf("buddy testing...\n");
    printf("options:\n");
    printf("[0]: test_buddy_malloc_with_merged\n");
    printf("[1]: test_buddy_free_merged\n");
    printf("[2]: test_buddy_malloc_size\n");
    printf("[e]: exit\n");
    buddy_status();
    printf("input option...");
    c = uart_getc();
    printf("\n");
    switch(c) {
    case 'e':
      return;
    case '0':
      test_buddy_malloc_with_merged();
      break;
    case '1':
      test_buddy_free_merged();
      break;
    case '2':
      test_buddy_malloc_size();
      break;
    default:
      printf("unknown command\n");
      break;
    }
  }
}

void test_buddy_malloc_size() {
  void* chunk1;
  int size;
  size = 0x1000;
  printf("test_buddy_malloc_size\n");
  
  chunk1 = buddy_malloc(size);
  memset((char* )chunk1, size, 'a');
  
  printf("%s\n", (char* )chunk1);

  buddy_status(); 
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  buddy_free(chunk1);

}