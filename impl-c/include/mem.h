#pragma once

#include "list.h"

// Configuration of the Buddy system
// Key conf:
// + How many phisycal frames does the Buddy system manage?
//    + @BUDDY_NUM_FRAMES
//    + @BUDDY_NUM_FRAMES = 2 ^ BUDDY_MAX_EXPONENT
// + What is the size of each frame?
//    + @BUDDY_FRAME_SHIFT
//     (Size of the frame in bytes = 2**BUDDY_FRAME_SHIFT)
#define BUDDY_NUM_FRAMES 8
#define BUDDY_MAX_EXPONENT 3
#define BUDDY_FRAME_SHIFT 0

#define BUDDY_NUM_FREE_LISTS (BUDDY_MAX_EXPONENT + 1)

// A Frame Object manage a physical page
typedef struct Frame {
  // inherit a list type, so we could cast FrameNode into list_head
  struct list_head list_base;

  int arr_index;
  int exp;
} Frame;

struct Frame Frames[BUDDY_NUM_FRAMES];

typedef struct BuddyAllocater {
  list_head_t free_lists[BUDDY_NUM_FREE_LISTS];
  Frame *frame_array;
} BuddyAllocater;

void buddy_init(BuddyAllocater *alloc, Frame *frame_arr);
int buddy_alloc(BuddyAllocater *alloc, int size_in_byte);
void buddy_free(BuddyAllocater *alloc, int addr);

void buddy_dump(BuddyAllocater *alloc);
