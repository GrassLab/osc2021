#pragma once

#include "list.h"

typedef struct FrameNode {
  // inherit a list type, so we could cast FrameNode into list_head
  struct list_head list_base;

  int arr_index;
  int exp;
} FrameNode;

// BUDDY_TOTAL_FRAMES = 2 ^ BUDDY_MAX_EXPONENT
#define BUDDY_NUM_FRAMES 8
#define BUDDY_MAX_EXPONENT 3

#define BUDDY_FRAME_SIZE 1

#define BUDDY_NUM_FREE_LISTS (BUDDY_MAX_EXPONENT + 1)

typedef struct BuddyAllocater {
  list_head_t free_lists[BUDDY_NUM_FREE_LISTS];
  FrameNode frame_array[BUDDY_NUM_FRAMES];
} BuddyAllocater;

void buddy_init(BuddyAllocater *self);
int buddy_alloc(BuddyAllocater *self, int size_in_byte);
void buddy_free(BuddyAllocater *self, int addr);

void dump_buddy(BuddyAllocater *self);

void FrameNode_init(FrameNode *self);