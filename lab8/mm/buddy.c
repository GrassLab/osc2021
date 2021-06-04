#include "buddy.h"
#include <printf.h>

void buddy_init() {
  //set buddy system start address, size, end address 
  buddy_system.start = (void* )BUDDY_START;
  buddy_system.size = (PAGE_SIZE)* BUDDY_BLOCK_NUM;
  buddy_system.end = buddy_system.start + buddy_system.size;
  //buddy header init 
  buddy_system.header[0].order = BUDDY_ORDER_MAX;
  buddy_system.header[0].addr = buddy_system.start;
  buddy_system.header[0].inuse = 0;
  for(int i = 1; i < BUDDY_BLOCK_NUM; i++) {
    buddy_system.header[i].addr = buddy_system.start + PAGE_SIZE * i;
    buddy_system.header[i].inuse = -1;
  }
  //free list init
  buddy_system.bins[BUDDY_ORDER_MAX] = &buddy_system.header[0];
}

void* buddy_malloc(size_t size) {
  struct buddy_block* block;
  int order;
  //get order from size
  for(order = 0; size > PAGE_SIZE*(1 << order); order++);

  if(order > BUDDY_ORDER_MAX) {
    uart_puts("request memory larger than memory pool.\n");
    return null;
  }

  block = buddy_find_free_block(order);
  
  if(block != null) {

    //init block 
    memset((char *)block->addr, PAGE_SIZE * (1 << order), 0);
  }
  
  return block->addr;
}
/**
 * find free block in free list
 * search order i list first, if not
 * find larger order
 */
void* buddy_find_free_block(int order) {
  
  struct buddy_block *find_block, *remain;
  
  if(order > BUDDY_ORDER_MAX) 
    return null;
  
  if(buddy_system.bins[order] != null) {
    //find free block
    find_block = buddy_system.bins[order];
    buddy_system.bins[order] = buddy_system.bins[order]->next;
    
    find_block->next = null;
    find_block->inuse = 1;
    buddy_update_inuse(find_block, 1);
    return find_block;
  }
  else 
    find_block = buddy_find_free_block(order + 1);
 
  if(find_block != null) {
    //get remain block
    remain = find_block + (1 << order);
    //set remain block
    remain->addr = find_block->addr + PAGE_SIZE * (1 << order);
    remain->order = order;
    remain->next = buddy_system.bins[order];
    buddy_system.bins[order] = remain;
    remain->inuse = 0;

    buddy_update_inuse(remain, -1);

    find_block->order = order;
  }
  return find_block;
}

void buddy_free(void* address) {
  
  if(address < buddy_system.start || address > buddy_system.end) 
    return;
  buddy_merge(address);
  //buddy_status();  
}

void buddy_merge(void* address) {
  struct buddy_block *buddy_block, *merged_block;
  size_t buddy_block_idx, merged_idx, merged_order;
  
  merged_idx = buddy_find_header_idx(address);
  
  //not valid idx
  if(merged_idx < 0 || merged_idx >= BUDDY_BLOCK_NUM)
    return;
  
  //initialize
  merged_block = &buddy_system.header[merged_idx];
  merged_order = merged_block->order;
  
  // merged can only be done when order <= BUDDY_ORDER_MAX - 1
  while(merged_order < BUDDY_ORDER_MAX) {
    buddy_block_idx = merged_idx ^ (1 << merged_order);
    buddy_block = &buddy_system.header[buddy_block_idx];
    //can be merged
    if(buddy_block->inuse != 1) {
      uart_puts("buddy is free.\n");
      //remove buddy block in free list
      if(buddy_remove_block(buddy_block, merged_order) == -1) {
        uart_puts("error: not found buddy block in free list.\n");
        //should not happened
        break;
      }
      //check the block num and buddy block num which is lower
      if(merged_idx > buddy_block_idx) {
        merged_idx = buddy_block_idx;
        merged_block = buddy_block;
      }
      merged_order += 1;
    }
    else 
      break;
  }
  //put block into free list 
  merged_block->next = buddy_system.bins[merged_order];
  buddy_system.bins[merged_order] = merged_block;
  merged_block->order = merged_order;
  merged_block->inuse = 0;
  buddy_update_inuse(merged_block, -1);
  
}

int buddy_find_header_idx(void* address) {
  /** 
   * linearly, should do more efficiently
   */
  for(int i = 0; i < BUDDY_BLOCK_NUM; i++) {

    if(buddy_system.header[i].addr == address)
      return i;
  }

  return null; 
}

int buddy_remove_block(void* address, int order) {
  struct buddy_block* block, *prev_block;
  block = buddy_system.bins[order];
  prev_block = null;

  //free list has only one element
  while(block != null) {
    if(block == (struct buddy_block* ) address) {
      //remove buddy block in free list
      if(prev_block != null)
        prev_block->next = block->next;
      else
        buddy_system.bins[order] = block->next;  
      /*uart_puts("remove block from free list order ");
      uart_hex(order);
      uart_puts(".\n");*/
      return 0;
    }
    prev_block = block;
    block = block->next;
  }
  //not found buddy block
  return -1;
}


void buddy_status() {
  buddy_puts_inuse();
  buddy_puts_free_list();
}

void buddy_puts_free_list() {
  uart_puts("free list info: \n");
  struct buddy_block* block;
  for(int i = 0; i <= BUDDY_ORDER_MAX; i++) {
    block = buddy_system.bins[i];
    if(block == null)
      continue;
    uart_puts("order [");
    uart_hex(i);
    uart_puts("]: ");
    while(block != null) {
      uart_hex((size_t)block->addr);
      uart_puts(" --> ");
      block = block->next;
    }
    uart_puts("null\n");
  }
}

void buddy_puts_inuse() {
  for(int i = 0; i < BUDDY_BLOCK_NUM; i++) {
    if(buddy_system.header[i].inuse == 1)
      uart_puts("X");
    else if(buddy_system.header[i].inuse == 0)
     printf("%d", buddy_system.header[i].order);
    else
      uart_puts("F");
  }
  uart_puts("\n");
}

void buddy_update_inuse(struct buddy_block* block, char c) {
  
  for(int i = 1; i < (1 << (block->order)); i++) {
    (block + i)->inuse = c;
  }

}

size_t buddy_get_blocknum_from_address(void* address) {
  return ((size_t)address - (size_t)buddy_system.start) / PAGE_SIZE;
}