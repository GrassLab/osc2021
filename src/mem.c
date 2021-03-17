#include "mem.h"

#include <stddef.h>

unsigned long long pool_top = 0x2000000;
unsigned long long current_top = 0x2000000;
unsigned long long pool_btm = 0x8000000;

void *kmalloc(unsigned long size) {
  void *new_chunk = (void *)current_top;
  current_top -= size;
  return new_chunk;
}

void kfree(void *ptr) {

}