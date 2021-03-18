#include "mem.h"
#include "io.h"

#include <stddef.h>

#define pad16(x) ((((x) + 15) / 16) * 16)
// #define pad16(x) ((((x) + 3) / 16) * 16)

unsigned long long pool_top = 0x2000000;
unsigned long long current_top = 0x2000000;
unsigned long long pool_btm = 0x8000000;

void *kmalloc(unsigned long size) {
  // if(size >= 4096) {
  //   current_top = 
  // }
  size = pad16(size);
  void *new_chunk = (void *)current_top;
  current_top += size;
  return new_chunk;
}

void kfree(void *ptr) {

}