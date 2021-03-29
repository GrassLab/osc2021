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
#define BUDDY_FRAME_SHIFT 14

#define BUDDY_NUM_FREE_LISTS (BUDDY_MAX_EXPONENT + 1)

#define MEMORY_START 0x90000

typedef struct Frame {
  // inherit a list type, so we could cast FrameNode into list_head
  struct list_head list_base;

  int arr_index;
  void *addr;
  int exp;
} Frame;

// BuddyAllocater
//    allocate contiguous frames
typedef struct BuddyAllocater {
  list_head_t free_lists[BUDDY_NUM_FREE_LISTS];
  Frame *frame_array;
} BuddyAllocater;

// // SlabAllocator
// //    manage slabs with the same allocation size,
// //    A slab is a frame allocated with small objects with same size
// //      + SLAB_MAX_SLOTS: The number of pages available for a single slab
// #define SLAB_MAX_SLOTS 256
// typedef struct SlabAllocator {
//   int unit_size; // size of the unit (in Bytes)
//   uint8_t usage[SLAB_MAX_SLOTS >> 3];

//   // Buffer indicate the current frames that would be use for obejct
//   allocation
//   // once buffer is fill, it would be added to fulled_list.
//   // Frames in fulled_list would be move to partial_list once an object is
//   freed struct Frame *buffer; list_head_t partial_list; list_head_t
//   fulled_list;
// } SlabAllocator;

typedef struct AllocationManager {
  // SlabAllocator obj_allocators[4]; // 16, 32, 48, 96
  BuddyAllocater *frame_allocator;
} AllocationManager;

void *kalloc(int blocks);
void kfree(void *addr);

// Initialize dynamic memory allocator
void KAllocManager_init();

void KAllocManager_show_status();

// Statically linked to the heap space
// because their lifetimes is equal to the system itself
struct Frame Frames[BUDDY_NUM_FRAMES];
struct AllocationManager KAllocManager;

void buddy_init(BuddyAllocater *alloc, Frame *frame_arr);
struct Frame *buddy_alloc(BuddyAllocater *alloc, int size_in_byte);
void buddy_free(BuddyAllocater *alloc, struct Frame *frame);
void buddy_dump(BuddyAllocater *alloc);
