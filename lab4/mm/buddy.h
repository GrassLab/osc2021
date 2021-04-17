#ifndef BUDDY_H
#define BUDDY_H
#include <types.h>
#include <mbox.h>
#include <string.h>

#define PAGE_SIZE (4 * 1024)
#define BUDDY_ORDER_MAX 8
#define BUDDY_BLOCK_NUM (1 << BUDDY_ORDER_MAX)
#define BUDDY_START  0x10000000 
#define BUDDY_HEADER_OFFSET 0x10
struct buddy_struct
{
  void* start;
  void* end;
  size_t size;
  //inuse array
  char inuse[BUDDY_BLOCK_NUM];
  //free frame linked list
  struct buddy_block *bins[BUDDY_ORDER_MAX + 1]; //including 2^0
} buddy_system;

struct buddy_block 
{
  size_t order;
  struct buddy_block *next;
};

void buddy_init();
void* buddy_malloc(size_t size);
void buddy_free(void* address);
void* buddy_find_free_block(int order);
void buddy_puts_free_list();
void buddy_update_inuse(void* block, int order);
void buddy_update_inuse_reset(void* block, int order);
void buddy_puts_inuse();
void buddy_status();
void buddy_merge(void* address);
int buddy_is_free(size_t idx, int order);
int buddy_remove_block(void* address, int order);
size_t buddy_get_blocknum_from_address(void* address);
#endif