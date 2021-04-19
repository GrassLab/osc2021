#include "varied.h"

void* varied_malloc(size_t size) {
  //check size to determine which allocator to use
  if(size > DYNAMIC_BIN_MAX * DYNAMIC_BIN_MIN_SLOT) 
    return buddy_malloc(size);
  else 
    return dynamic_malloc(size);
}

void varied_free(void* address) {
  size_t size, prev_size, order;
  void* next;

  size = ((struct dynamic_chunk* )(address - DYNAMIC_CHUNK_HEADER_OFFSET))->size;
  prev_size = ((struct dynamic_chunk* )(address - DYNAMIC_CHUNK_HEADER_OFFSET))->prev_size;
  next = ((struct dynamic_chunk* )(address - DYNAMIC_CHUNK_HEADER_OFFSET))->next;
  
  //check header to determine which free allocator to use
  if(size > 0x20 && (size & 0x1)) {
    if(prev_size == 0 || prev_size % DYNAMIC_BIN_MIN_SLOT == 0) {
      if(next == 0) {
        //dynamic
        dynamic_free(address);
        return;
      }
    }
  }

  order = ((struct buddy_block *)(address - BUDDY_HEADER_OFFSET))->order;
  next = ((struct buddy_block* )(address - BUDDY_HEADER_OFFSET))->next;
  
  if(order >= 0 && order <= BUDDY_ORDER_MAX) {
    if(next == 0) {
      //buddy
      buddy_free(address);
      return;
    }
  }
}


