#include "dynamic.h"
#include <buddy.h>

void dynamic_init() {
  if(dynamic_system.top_chunk == null) {
    //request page frame from buddy
    dynamic_system.top_chunk = buddy_malloc(PAGE_SIZE);
    dynamic_system.top_chunk->size = PAGE_SIZE;
    dynamic_system.top_chunk->next = null;
    dynamic_system.top_chunk->prev_size = 0;
  }
}

void* dynamic_malloc(size_t size) {
  void* chunk;
  int bin_idx;
  //get nearest bin size from size
  for(bin_idx = 0; size > (bin_idx + 1) * DYNAMIC_BIN_MIN_SIZE; bin_idx++);
  uart_puts("bin size: ");
  uart_hex((bin_idx + 1) * DYNAMIC_BIN_MIN_SIZE);
  uart_puts("\n");
  if(bin_idx > DYNAMIC_BIN_MAX) {
    uart_puts("request memory larger than memory pool.\n");
    return null;
  }
  //check free list
  chunk = dynamic_find_free_chunk(bin_idx);
  //check top chunk
  if(chunk == null) {
    if(dynamic_system.top_chunk->size < (bin_idx + 1) * DYNAMIC_BIN_MIN_SIZE) {
      //request new page frame from chunk
      uart_puts("allocate new page frame.\n");
      void* tmp = buddy_malloc(PAGE_SIZE);
      if(tmp == null) {
        uart_puts("request new page failed.\n ");
        return null;
      }
      //put rest of top chunk into free list
      dynamic_top_chunk_free();
      //set to new top chunk
      dynamic_system.top_chunk = tmp;
      dynamic_system.top_chunk->size = PAGE_SIZE;
      dynamic_system.top_chunk->next = null;
      dynamic_system.top_chunk->prev_size = 0;
    }
    //allocate from top chunk
    uart_puts("allocate from top chunk.\n");
    chunk = dynamic_top_chunk_malloc(bin_idx);
  }
  dynamic_status();
  uart_puts("allocated address: ");
  uart_hex((size_t)chunk);
  uart_puts("\n");
  uart_puts("allocated size ");
  uart_hex(((struct dynamic_chunk* )chunk)->size);
  uart_puts("\n");
  return chunk;
}

void* dynamic_find_free_chunk(int idx) {
  void* chunk = null;
  int find_idx = idx;
  while(find_idx < DYNAMIC_BIN_MAX) {
    if(dynamic_system.bins[find_idx] != null) {
      uart_puts("find free chunk in ");
      uart_hex((find_idx + 1) * DYNAMIC_BIN_MIN_SIZE);
      uart_puts(".\n");
      //find free chunk
      chunk = dynamic_system.bins[find_idx];
      dynamic_system.bins[find_idx] = dynamic_system.bins[find_idx]->next;
      //set chunk header
      ((struct dynamic_chunk *)chunk)->size = (idx + 1) * DYNAMIC_BIN_MIN_SIZE + 1; 
      ((struct dynamic_chunk *)chunk)->next = null; 
      if(find_idx > idx) {
        //split chunk
        int split_idx = (find_idx - idx) - 1;
        struct dynamic_chunk* split_chunk = (void *)chunk + (idx + 1) * DYNAMIC_BIN_MIN_SIZE;
        split_chunk->size = (find_idx - idx + 1) * DYNAMIC_BIN_MIN_SIZE;
        split_chunk->next = dynamic_system.bins[split_idx];
        split_chunk->prev_size = (idx + 1) * DYNAMIC_BIN_MIN_SIZE;
        dynamic_system.bins[split_idx] = split_chunk;
        uart_puts("split free chunk to size ");
        uart_hex((find_idx - idx + 1) * DYNAMIC_BIN_MIN_SIZE);
        uart_puts(".\n");
      }
      return chunk;
    }
    find_idx++;
  }
  return chunk;
}

void* dynamic_top_chunk_malloc(int idx) {
  void* chunk;
  size_t chunk_size, top_chunk_size;
  
  top_chunk_size = dynamic_system.top_chunk->size;
  chunk = dynamic_system.top_chunk;
  chunk_size = (idx + 1) * DYNAMIC_BIN_MIN_SIZE;
  dynamic_system.top_chunk->size = chunk_size + 1; 
  dynamic_system.top_chunk = (void *)dynamic_system.top_chunk + chunk_size;
  dynamic_system.top_chunk->size = top_chunk_size - chunk_size;
  dynamic_system.top_chunk->prev_size = chunk_size;
  return chunk;
}

void dynamic_free(void* address) {
  int merged_idx;
  struct dynamic_chunk *chunk, *prev_chunk, *next_chunk, *next_next_chunk;
  
  if(address < buddy_system.start || address > buddy_system.end) 
    return;
  
  chunk = address;
  chunk->size -= 1; //inuse bit
  merged_idx = chunk->size / DYNAMIC_BIN_MIN_SIZE - 1;
  uart_puts("free size ");
  uart_hex(chunk->size);
  uart_puts("\n");
  uart_puts("prev size ");
  uart_hex(chunk->prev_size);
  uart_puts("\n");
  if(chunk->size > 0 && chunk->size <= DYNAMIC_BIN_MAX * DYNAMIC_BIN_MIN_SIZE) {
    prev_chunk = (void*)chunk - chunk->prev_size;
    next_chunk = (void*)chunk + chunk->size;
    if(chunk->prev_size != 0 && (prev_chunk->size & 0x1) == 0) {
      //prev chunk is freed
      uart_puts("prev chunk is freed.\n");
      if(dynamic_remove_chunk(prev_chunk, prev_chunk->size) == -1) {
        uart_puts("not found chunk.\n");
        return;
      }
      //merge chunk
      merged_idx = (chunk->size + prev_chunk->size) / DYNAMIC_BIN_MIN_SIZE - 1;
      prev_chunk->size = chunk->size + prev_chunk->size;
      chunk = prev_chunk;
      //set next chunk prev size
      next_next_chunk = (void*)chunk + chunk->size;
      next_next_chunk->prev_size = chunk->size; 
    }

    if((next_chunk->size & 0x1) == 0) {
      //next chunk is free
      if(next_chunk == dynamic_system.top_chunk) {
        //is top chunk
        uart_puts("next chunk is top chunk.\n");
        //update top chunk
        chunk->size = chunk->size + dynamic_system.top_chunk->size;
        chunk->next = null;
        dynamic_system.top_chunk = chunk;
        dynamic_status();  
        return;
      }
      else {
        uart_puts("next chunk is freed.\n");
        if(dynamic_remove_chunk(next_chunk, next_chunk->size) == -1) {
          uart_puts("not found chunk.\n");
          return;
        }
        //merge chunk
        merged_idx = (chunk->size + next_chunk->size) / DYNAMIC_BIN_MIN_SIZE - 1; 
        chunk->size = chunk->size + next_chunk->size;
        //set next chunk prev size
        next_next_chunk = (void*)chunk + chunk->size;
        next_next_chunk->prev_size = chunk->size; 
      }
    }
    //put into free list
    chunk->next = dynamic_system.bins[merged_idx];
    dynamic_system.bins[merged_idx] = chunk;  
  }
  dynamic_status();  
}

void dynamic_status() {
  uart_puts("dynamic_status:\n");
  uart_puts("free list info: \n");
  struct dynamic_chunk* chunk;
  for(int i = 0; i < DYNAMIC_BIN_MAX; i++) {
    chunk = dynamic_system.bins[i];
    if(chunk == null)
      continue;
    uart_puts("size [");
    uart_hex((i + 1) * DYNAMIC_BIN_MIN_SIZE);
    uart_puts("]: ");
    while(chunk != null) {
      uart_hex((size_t)chunk);
      uart_puts(" --> ");
      chunk = chunk->next;
    }
    uart_puts("null\n");
  }
  uart_puts("top chunk info: \n");
   uart_puts("address: ");
   uart_hex((size_t)dynamic_system.top_chunk);
   uart_puts("\n");
   uart_puts("size: ");
   uart_hex(dynamic_system.top_chunk->size);
   uart_puts("\n");
}

void dynamic_top_chunk_free() {
  size_t size;
  int idx;
  size = dynamic_system.top_chunk->size;
  if(size > 0 && size <= DYNAMIC_BIN_MAX * DYNAMIC_BIN_MIN_SIZE) {
    idx = size / DYNAMIC_BIN_MIN_SIZE - 1;
    dynamic_system.top_chunk->next = dynamic_system.bins[idx];
    dynamic_system.bins[idx] = dynamic_system.top_chunk;
  }
}

int dynamic_remove_chunk(void* address, size_t size) {
  int idx = size / DYNAMIC_BIN_MIN_SIZE - 1;
  struct dynamic_chunk* chunk = dynamic_system.bins[idx];
  //free list has only one element
  if(chunk != null && (chunk == (struct dynamic_chunk* ) address)) {
    uart_puts("remove chunk from free list size ");
    uart_hex(size);
    uart_puts(".\n");
    dynamic_system.bins[idx] = chunk->next;
    return 0;
  }
  
  while(chunk != null) {
    if(chunk->next == (struct dynamic_chunk* ) address) {
      //remove chunk in free list
      chunk->next = ((struct dynamic_chunk* )address)->next;
      uart_puts("remove chunk from free list size ");
      uart_hex(size);
      uart_puts(".\n");
      return 0;
    }
    chunk = chunk->next;
  }
  //not found chunk
  return -1;
}