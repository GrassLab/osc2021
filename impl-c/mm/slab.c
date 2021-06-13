#include "bool.h"
#include "list.h"
#include "log.h"
#include "mm.h"
#include "uart.h"
#include <stddef.h>

#ifdef CFG_LOG_MEM_SLAB
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

void *slab_alloc(SlabAllocator *alloc) {
  Frame *frame;

  if (alloc->cur_frame == NULL) {
    if (!list_empty(&alloc->partial_list)) {
      frame = (Frame *)list_pop(&alloc->partial_list);
      alloc->cur_frame = frame;
      log_println("slab: Recycle frame from partial filled slab");
    } else {
      frame = buddy_alloc(alloc->frame_allocator, 0);
      if (frame == NULL)
        return NULL;
      log_println("slab: Request frame from buddy system");
      frame->slab_allocator = alloc;
      alloc->cur_frame = frame;
      for (int i = 0; i < SLAB_MAX_SLOTS; i++) {
        frame->slot_available[i] = 1;
      }
      frame->free_slot_remains = alloc->max_slab_num_obj;
      // frame->free_slot_remains = 5;
      log_println("  slot remains: %d, max:%d", frame->free_slot_remains,
                  alloc->max_slab_num_obj);
    }
  }
  frame = alloc->cur_frame;

  void *addr;

  // find a space for allocation
  for (int i = 0; i < SLAB_MAX_SLOTS; i++) {
    if (frame->slot_available[i] == 1) {
      addr = frame->addr + i * alloc->unit_size;
      frame->slot_available[i] = 0;
      frame->free_slot_remains -= 1;
      break;
    }
  }

  if (frame->free_slot_remains <= 0) {
    // if the page is full, move it to the full-list
    list_push(&frame->list_base, &alloc->full_list);
    alloc->cur_frame = NULL;
    log_println("==== slab frame is full");
  }
  return addr;
}

void slab_free(void *obj) {
  // get frame address first
  int arr_index = (long)(obj - MEMORY_START) >> FRAME_SHIFT;
  struct Frame *frame = &Frames[arr_index];
  struct SlabAllocator *alloc = frame->slab_allocator;

  int obj_index = (((long)(obj - MEMORY_START) & ((1 << FRAME_SHIFT) - 1)) /
                   alloc->unit_size);

  if (frame->slot_available[obj_index] == 1) {
    log_println("!!Free after free");
  }
  log_println("slab: Free object");
  frame->slot_available[obj_index] = 1;
  frame->free_slot_remains += 1;
  if (frame != alloc->cur_frame) {
    list_del(&frame->list_base);
    if (frame->slot_available > 0) {
      list_push(&frame->list_base, &alloc->partial_list);
    } else {
      log_println("slab: release block");
      buddy_free(alloc->frame_allocator, frame);
    }
  }
}
