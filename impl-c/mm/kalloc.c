#include "bool.h"
#include "list.h"
#include "log.h"
#include "mm.h"
#include "mm/startup.h"
#include "uart.h"
#include <stddef.h>

#ifdef CFG_LOG_MEM_KALLOC
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

struct AllocationManager KAllocManager;
struct Frame Frames[BUDDY_MAX_EXPONENT << 1];

void KAllocManager_run_example() {
  void *a[30];
  uart_println("[Example] Allocate 5 single frames:");
  for (int i = 0; i < 5; i++) {
    a[i] = kalloc(8192);
    uart_println("i:%d, a: %x", i, a[i]);
  }

  uart_println("[Example] free 5 frames we've just allocated");
  for (int i = 0; i < 5; i++) {
    kfree(a[i]);
  }

  uart_println("[Example] print 5 objects with 13 bytes each");
  for (int i = 0; i < 5; i++) {
    a[i] = kalloc(13);
    uart_println("i:%d, a: %x", i, a[i]);
  };
  uart_println("[Example] free 5 frames we've just allocated");
  for (int i = 0; i < 5; i++) {
    kfree(a[i]);
  }
}

void KAllocManager_init() {
  AllocationManager *am = &KAllocManager;
  buddy_init(&am->frame_allocator, &StartupAlloc, Frames);

  for (int i = 0; i < (1 << BUDDY_MAX_EXPONENT); i++) {
    for (int j = 0; j < (SLAB_MAX_SLOTS >> 3); j++) {
      Frames[i].slot_available[j] = 0;
    }
    Frames[i].slab_allocator = NULL;
    Frames[i].free_slot_remains = 0;
  }

  SlabAllocator *slab_alloc;
  for (int i = SLAB_OBJ_MIN_SIZE_EXP; i <= SLAB_OBJ_MAX_SIZE_EXP; i++) {
    slab_alloc = &am->obj_allocator_list[i - SLAB_OBJ_MIN_SIZE_EXP];
    slab_alloc->unit_size = 1 << i;
    slab_alloc->max_slab_num_obj = 4096 / slab_alloc->unit_size;
    slab_alloc->frame_allocator = &am->frame_allocator;
    slab_alloc->cur_frame = NULL;
    list_init(&slab_alloc->partial_list);
    list_init(&slab_alloc->full_list);
  }
}

void KAllocManager_show_status() { buddy_dump(&KAllocManager.frame_allocator); }

void *kalloc(int size) {
  void *addr;
  if (size <= (1 << SLAB_OBJ_MAX_SIZE_EXP)) {
    for (int i = SLAB_OBJ_MIN_SIZE_EXP; i < SLAB_OBJ_MAX_SIZE_EXP; i++) {
      if (size < 1 << i) {
        log_println("Allocation from slab allocator, size: %d", size);
        addr = slab_alloc(
            &KAllocManager.obj_allocator_list[i - SLAB_OBJ_MIN_SIZE_EXP]);
        return addr;
      }
    }
  }
  // allcation using buddy system
  for (int i = 0; i < BUDDY_MAX_EXPONENT; i++) {
    if ((i << FRAME_SHIFT) >= size) {
      Frame *frame = buddy_alloc(&KAllocManager.frame_allocator, i);
      log_println("Allocate Request exp:%d", i);
      log_println("Allocated addr:%x, frame_idx:%d", frame->addr,
                  frame->arr_index);
      return frame->addr;
    }
  }
  return NULL;
}
void kfree(void *addr) {
  // Get frame from address provided
  int arr_index = (long)(addr - MEMORY_START) >> FRAME_SHIFT;
  Frame *frame = &Frames[arr_index];
  if (frame->slab_allocator) {
    slab_free(addr);
  } else {
    log_println("Free Request addr:%x, frame_idx:%d", frame->addr,
                frame->arr_index);
    buddy_free(&KAllocManager.frame_allocator, frame);
  }
}
