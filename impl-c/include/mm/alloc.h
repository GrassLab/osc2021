#pragma once

#include "list.h"
#include "mm/const.h"
#include "mm/frame.h"
#include "mm/startup.h"
#include <stdint.h>

typedef struct Frame {
  // inherit a list type, so we could cast FrameNode into list_head
  struct list_head list_base;

  // only if this page is slab allocated if this field to be useful
  uint8_t slot_available[SLAB_MAX_SLOTS];
  struct SlabAllocator *slab_allocator;
  int free_slot_remains;

  int arr_index;
  void *addr;
  int exp;
} Frame;

// BuddyAllocater
//    allocate contiguous frames
typedef struct BuddyAllocater {
  list_head_t free_lists[BUDDY_NUM_FREE_LISTS];
  struct Frame *frames;
} BuddyAllocater;

typedef struct SlabAllocator {
  int unit_size; // size of the unit (in Bytes)
  int max_slab_num_obj;

  BuddyAllocater *frame_allocator;

  struct Frame *cur_frame;  // The current frame be used for obejct allocation
  list_head_t partial_list; // Frames in full_list would be move to
                            //  partial_list once an object is freed
  list_head_t full_list;    // Once cur_frame is full, it would be dispatch to
                            //  full_list for storage.
} SlabAllocator;

// Call slab allocator for allocate an object
void *slab_alloc(SlabAllocator *alloc);

// Free an object
void slab_free(void *obj);

void buddy_init(BuddyAllocater *alloc, StartupAllocator_t *sa,
                struct Frame *frames);
struct Frame *buddy_alloc(BuddyAllocater *alloc, int size_in_byte);
void buddy_free(BuddyAllocater *alloc, struct Frame *frame);
void buddy_dump(BuddyAllocater *alloc);
