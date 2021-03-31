#pragma once

#include "utils.h"

#define PAGE_BASE_ADDR ((uint64_t)0x10000000)
#define PAGE_END_ADDR ((uint64_t)0x20000000)
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
  struct PageFrame *prev, *next;
} page_frame;

page_frame frames[MAX_PAGE_NUM];
page_frame *free_frame_lists[FRAME_LIST_NUM], *used_frame_lists[FRAME_LIST_NUM];

void buddy_init();
void buddy_test();
uint64_t buddy_allocate(uint64_t size);
void buddy_free(uint64_t addr);
void unlink(int index, int type);
void print_frame_lists();
