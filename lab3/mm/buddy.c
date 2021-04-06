#include "buddy.h"

void buddy_init() {
  //set buddy system start address, size, end address 
  buddy_system.start = (void *)BUDDY_START;
  buddy_system.size = PAGE_SIZE * BUDDY_BLOCK_NUM;
  buddy_system.end = buddy_system.start + buddy_system.size;
  //buddy header init 
  struct buddy_block* block = buddy_system.start;
  block->next = null;
  block->order = BUDDY_ORDER_MAX;
  //free list init
  buddy_system.bins[BUDDY_ORDER_MAX] = buddy_system.start;
  //inuse map init 
  buddy_system.inuse[0] = BUDDY_ORDER_MAX; 
  memset(buddy_system.inuse + 1, BUDDY_BLOCK_NUM - 1, 'F');  
}

void* buddy_malloc(size_t size) {
  void* block;
  int order;
  //get order from size
  for(order = 0; size > PAGE_SIZE*(1 << order); order++);

  if(order > BUDDY_ORDER_MAX) {
    uart_puts("request memory larger than memory pool.\n");
    return null;
  }
  block = buddy_find_free_block(order);
  if(block != null) {
    //set block header
    ((struct buddy_block* )block)->order = order;
    ((struct buddy_block* )block)->next = null;
    //set inuse map
    buddy_update_inuse(block, order);
    //init block 
    memset((char *)(block + BUDDY_HEADER_OFFSET), PAGE_SIZE * (1 << order) - BUDDY_HEADER_OFFSET, 0);
    uart_puts("allocated address: ");
    uart_hex((size_t)block);
    uart_puts("\n");

    uart_puts("order: ");
    uart_hex(((struct buddy_block* )block)->order);
    uart_puts("\n");
  }
  buddy_status();
  return block + BUDDY_HEADER_OFFSET;
}
/**
 * find free block in free list
 * search order i list first, if not
 * find larger order
 */
void* buddy_find_free_block(int order) {
  void* find_block;
  if(order > BUDDY_ORDER_MAX) 
    return null;
  if(buddy_system.bins[order] != null) {
    //find free block
    find_block = buddy_system.bins[order];
    buddy_system.bins[order] = buddy_system.bins[order]->next;
    return find_block;
  }
  else 
    find_block = buddy_find_free_block(order + 1);
 
  if(find_block != null) {
    uart_puts("release redundant memory block from order ");
    uart_hex(order + 1);
    uart_puts(" to order ");
    uart_hex(order);
    uart_puts(".\n");
    //put order i+1 half of block into order i free list 
    struct buddy_block* remain = (void *)find_block + PAGE_SIZE * (1 << order);
    remain->next = buddy_system.bins[order];
    buddy_system.bins[order] = remain;
    //update inuse map
    buddy_system.inuse[buddy_get_blocknum_from_address(remain)] = order;
    memset(buddy_system.inuse + buddy_get_blocknum_from_address(remain) + 1, (1 << order) - 1, 'F');
    //buddy_puts_inuse();
  }
  return find_block;
}

void buddy_free(void* address) {
  address -= BUDDY_HEADER_OFFSET;
  if(address < buddy_system.start || address > buddy_system.end) 
    return;
  buddy_merge(address);
  buddy_status();  
}

void buddy_merge(void* address) {
  struct buddy_block *buddy_block, *merged_block;
  size_t buddy_block_idx, merged_idx, merged_order;
  //initialize
  merged_block = address;
  merged_idx = buddy_get_blocknum_from_address(merged_block);
  merged_order = merged_block->order;
  // merged can only be done when order <= BUDDY_ORDER_MAX - 1
  while(merged_order < BUDDY_ORDER_MAX) {
    buddy_block_idx = merged_idx ^ (1 << merged_order);
    buddy_block = (void *)BUDDY_START + PAGE_SIZE * buddy_block_idx;
    //can be merged
    if(buddy_is_free(buddy_block_idx, merged_order) == 0) {
      uart_puts("buddy is free.\n");
      //remove buddy block in free list
      if(buddy_remove_block(buddy_block, merged_order) == -1) {
        uart_puts("error: not found buddy block in free list.\n");
        return;
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
  //update inuse map
  buddy_update_inuse_reset(merged_block, merged_order);
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
      uart_puts("remove block from free list order ");
      uart_hex(order);
      uart_puts(".\n");
      return 0;
    }
    prev_block = block;
    block = block->next;
  }
  //not found buddy block
  return -1;
}
int buddy_is_free(size_t idx, int order) {
  size_t size = (1 << order);
  for(int i = 0; i < size; i++) {
    if(buddy_system.inuse[idx + i] == 'X')
      return 1;
  }
  return 0;
}
void buddy_status() {
  /*uart_puts("buddy status:\n");
  uart_puts("start address: ");
  uart_hex((size_t)buddy_system.start);
  uart_puts("\nend address: ");
  uart_hex((size_t)buddy_system.end);
  uart_puts("\n");*/
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
      uart_hex((size_t)block);
      uart_puts(" --> ");
      block = block->next;
    }
    uart_puts("null\n");
  }
}

void buddy_puts_inuse() {
  uart_puts("inuse info: \n");
  for(int i = 0; i < BUDDY_BLOCK_NUM; i++) {
    if(buddy_system.inuse[i] <= 8)
      uart_send(buddy_system.inuse[i] + 0x30);
    else
      uart_send(buddy_system.inuse[i]);
  }
  uart_puts("\n");
}
//update alocate
void buddy_update_inuse(void* block, int order) {
  size_t start = buddy_get_blocknum_from_address(block);
  size_t size = (1 << order);
  memset(buddy_system.inuse + start, size, 'X');
  
}
//update free
void buddy_update_inuse_reset(void* block, int order) {
  size_t start = buddy_get_blocknum_from_address(block);
  size_t size = (1 << order);
  buddy_system.inuse[start] = order;
  memset(buddy_system.inuse + start + 1, size - 1, 'F');
}
size_t buddy_get_blocknum_from_address(void* address) {
  return ((size_t)address - BUDDY_START)/ PAGE_SIZE;
}