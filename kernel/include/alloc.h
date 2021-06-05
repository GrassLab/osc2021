#pragma once

#include <stdint.h>

#include "mmu.h"
#include "utils.h"

#define PAGE_BASE_ADDR ((uint64_t)(KVA + 0x10000000))
#define PAGE_END_ADDR ((uint64_t)(KVA + 0x20000000))
#define PAGE_SIZE ((uint64_t)(4 * kb))
#define MAX_PAGE_NUM \
  ((uint64_t)((PAGE_END_ADDR - PAGE_BASE_ADDR) / PAGE_SIZE))  // 65536
#define MAX_FRAME_ORDER 16                                    // 2^16 = 65536
#define FRAME_LIST_NUM (MAX_FRAME_ORDER + 1)

typedef struct PageFrame {
  int id;
  int order;
  int is_allocated;
  uint64_t addr;
  struct PageFrame *next;
} page_frame;

typedef struct DMAHeader {
  uint64_t total_size;
  uint64_t used_size;
  int is_allocated;
  page_frame *frame_ptr;
  struct DMAHeader *prev, *next;
} dma_header;

page_frame frames[MAX_PAGE_NUM];
page_frame *free_frame_lists[FRAME_LIST_NUM], *used_frame_lists[FRAME_LIST_NUM];

dma_header *free_dma_list;

void buddy_init();
void buddy_test();
page_frame *buddy_allocate(uint64_t size);
void buddy_free(page_frame *frame);
void buddy_unlink(int index, int type);
void print_frame_lists();
void dma_test();
void *malloc(uint64_t size);
void free(void *ptr);
void print_dma_list();
