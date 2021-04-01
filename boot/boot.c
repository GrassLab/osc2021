#include <stdbool.h>

#include "buddy/buddy.h"
#include "memory/memory.h"
#include "printf/printf.h"
#include "uart/uart.h"

void _putchar(char character) {
  if (character == '\n') {
    uart_send('\r');
  }
  uart_send(character);
}

int main() {
  uart_init();
  buddy_init();
  memory_init();
  while (true) {
    char c = uart_getc();
    if (c != '\n') {
      continue;
    }
    //
    buddy_t block1 = buddy_alloc(3);
    printf("buddy index %d\n", block1.index);
    buddy_t block2 = buddy_alloc(5);
    printf("buddy index %d\n", block2.index);
    buddy_free(block1);
    buddy_free(block2);
    //
    void* ptr1 = memory_alloc(100);
    printf("memory address %p\n", ptr1);
    void* ptr2 = memory_alloc(100);
    printf("memory address %p\n", ptr2);
    void* ptr3 = memory_alloc(1000);
    printf("memory address %p\n", ptr3);
    memory_free(ptr1);
    memory_free(ptr2);
    memory_free(ptr3);
    printf("\n");
  }
  return 0;
}
