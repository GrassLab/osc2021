#include "dynamic_test.h"
#include <dynamic.h>
#include <printf.h>
#include <uart.h>
#include <reset.h>
#include <string.h>

void test_dynamic_free_prev_merge() {
  void* chunk1, *chunk2, *chunk3;
  int size;
  printf("test_dynamic_free_prev_merge...\n");
  
  size = 0x40;
  chunk1 = dynamic_malloc(size); 
  chunk2 = dynamic_malloc(size); 
  chunk3 = dynamic_malloc(size); 

  dynamic_free(chunk1);
  dynamic_free(chunk2);
  
  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");

  dynamic_free(chunk3);
}

void test_dynamic_free_next_merge() {
  void* chunk1, *chunk2, *chunk3;
  int size;
  size = 0x40;
  printf("test_dynamic_free_next_merge...\n");

  chunk1 = dynamic_malloc(size); 
  chunk2 = dynamic_malloc(size); 
  chunk3 = dynamic_malloc(size); 

  dynamic_free(chunk2);
  dynamic_free(chunk1);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");

  dynamic_free(chunk3);
    
}

void test_dynamic_free_both_merge() {
  void* chunk1, *chunk2, *chunk3, *chunk4;
  int size;
  size = 0x40;
  printf("test_dynamic_free_both_merge...\n");
  
  chunk1 = dynamic_malloc(size); 
  chunk2 = dynamic_malloc(size); 
  chunk3 = dynamic_malloc(size); 
  chunk4 = dynamic_malloc(size);
  
  dynamic_free(chunk1);
  dynamic_free(chunk3);
  dynamic_free(chunk2);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");

  dynamic_free(chunk4);
}

void test_dynamic_free_top_chunk_merge() {
  void* chunk1;
  int size;
  size = 0x40;
  chunk1 = dynamic_malloc(size);
  printf("test_dynamic_free_top_chunk_merge...\n");
  
  dynamic_free(chunk1);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
}

void test_dynamic_free_merge_to_unsorted_bin() {
  void* chunk1, *chunk2, *chunk3;
  int size;
  size = 0x400;
  printf("test_dynamic_free_merge_to_unsorted_bin...\n");
  
  chunk1 = dynamic_malloc(size);
  chunk2 = dynamic_malloc(size);
  chunk3 = dynamic_malloc(size);

  dynamic_free(chunk1);
  dynamic_free(chunk2);
   
  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  dynamic_free(chunk3);
}

void test_dynamic_main() {
  char c;
  while(1) {
    printf("dynamic testing...\n");
    printf("options:\n");
    printf("[0]: test_dynamic_free_prev_merge\n");
    printf("[1]: test_dynamic_free_next_merge\n");
    printf("[2]: test_dynamic_free_both_merge\n");
    printf("[3]: test_dynamic_free_top_chunk_merge\n");
    printf("[4]: test_dynamic_malloc_size\n");
    printf("[5]: test_dynamic_find_free_chunk\n");
    printf("[6]: test_dynamic_free_merge_to_unsorted_bin\n");
    printf("[7]: test_dynamic_unsorted_bin_malloc_split_to_bin_list\n");
    printf("[8]: test_dynamic_unsorted_bin_malloc_split_to_unsorted_bin\n");
    printf("[9]: test_dynamic_request_new_page\n");
    printf("[e]: exit\n");
    dynamic_status();
    printf("input option...");
    c = uart_getc();
    printf("\n");
    switch(c) {
    case 'e':
      return;
    case '0':
      test_dynamic_free_prev_merge();
      break;
    case '1':
      test_dynamic_free_next_merge();
      break;
    case '2':
      test_dynamic_free_both_merge();
      break;
    case '3':
      test_dynamic_free_top_chunk_merge();
      break;
    case '4':
      test_dynamic_malloc_size();
      break;
    case '5':
      test_dynamic_find_free_chunk();
      break;
    case '6':
      test_dynamic_free_merge_to_unsorted_bin();
      break;
    case '7':
      test_dynamic_unsorted_bin_malloc_split_to_bin_list();
    case '8':
      test_dynamic_unsorted_bin_malloc_split_to_unsorted_bin();
    case '9':
      test_dynamic_request_new_page();
    default:
      printf("unknown command\n");
      break;
    }
  }
}

void test_dynamic_malloc_size() {
  char* chunk1;
  int size;
  size = 0x10;  
  printf("test_dynamic_malloc_size...\n");
  chunk1 = (char *)dynamic_malloc(size);
  
  memset(chunk1, size, 'a');
  printf("%s\n", chunk1);
  
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  dynamic_free(chunk1);
}

void test_dynamic_find_free_chunk() {
  void* chunk1, *chunk2;
  int size;
  size = 0x40;
  printf("test_dynamic_find_free_chunk...\n");
  
  chunk1 = dynamic_malloc(size);
  chunk2 = dynamic_malloc(size);
  dynamic_free(chunk1);
  
  dynamic_status();
  chunk1 = dynamic_malloc(size);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  dynamic_free(chunk1);
  dynamic_free(chunk2);
}

void test_dynamic_unsorted_bin_malloc_split_to_bin_list() {
  void* chunk1, *chunk2, *chunk3;
  int size1, size2;
  size1 = 0x400;
  size2 = 0x40; 
  printf("test_dynamic_unsorted_bin_malloc_split_to_bin_list...\n");

  chunk1 = dynamic_malloc(size1);
  chunk2 = dynamic_malloc(size2);
  chunk3 = dynamic_malloc(size1);
  
  dynamic_free(chunk1);
  dynamic_free(chunk2);

  dynamic_status();
  chunk1 = dynamic_malloc(size1);
  
  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
   
  dynamic_free(chunk1);
  dynamic_free(chunk3);

}

void test_dynamic_unsorted_bin_malloc_split_to_unsorted_bin() {
  void* chunk1, *chunk2, *chunk3;
  int size1, size2;
  size1 = 0x400;
  size2 = 0x40; 
  printf("test_dynamic_unsorted_bin_malloc_split_to_unsorted_bin...\n");

  chunk1 = dynamic_malloc(size1);
  chunk2 = dynamic_malloc(size1);
  chunk3 = dynamic_malloc(size1);
  
  dynamic_free(chunk1);
  dynamic_free(chunk2);
  
  dynamic_status();
  chunk1 = dynamic_malloc(size2);
  
  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
   
  dynamic_free(chunk1);
  dynamic_free(chunk3);

}

void test_dynamic_request_new_page() {
  void* chunk1, *chunk2, *chunk3, *chunk4;
  int size;
  size = 0x400;
  printf("test_dynamic_request_new_page...\n");

  chunk1 = dynamic_malloc(size);
  chunk2 = dynamic_malloc(size);
  chunk3 = dynamic_malloc(size);
  
  dynamic_status();
  chunk4 = dynamic_malloc(size);

  dynamic_status();
  printf("press any key to continue...");
  uart_getc();
  uart_puts("\n");
  
  dynamic_free(chunk1);
  dynamic_free(chunk2);
  dynamic_free(chunk3);
  dynamic_free(chunk4);

}