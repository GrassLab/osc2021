#ifndef BUDDY_H
#define BUDDY_H
#include <types.h>
#include <mbox.h>
#include <string.h>

#define PAGE_SIZE (4 * 1024)
#define BUDDY_ORDER_MAX 9
#define BUDDY_BLOCK_NUM (1 << BUDDY_ORDER_MAX)
#define BUDDY_START  0xffff000010000000 
#define BUDDY_HEADER_OFFSET 0x10

struct buddy_block 
{
  size_t order;
  void* addr;
  struct buddy_block *next;
  char inuse;
};

struct buddy_struct
{
  void* start;
  void* end;
  size_t size;
  //inuse array
  struct buddy_block header[BUDDY_BLOCK_NUM];
  //free frame linked list
  struct buddy_block *bins[BUDDY_ORDER_MAX + 1]; //including 2^0
} buddy_system;

void buddy_init();
void* buddy_malloc(size_t size);
void buddy_free(void* address);
void* buddy_find_free_block(int order);
void buddy_puts_free_list();
void buddy_puts_inuse();
void buddy_status();
int buddy_find_header_idx(void* address);
void buddy_merge(void* address);
int buddy_remove_block(void* address, int order);
size_t buddy_get_blocknum_from_address(void* address);
void buddy_update_inuse(struct buddy_block* block, char c);
#endif