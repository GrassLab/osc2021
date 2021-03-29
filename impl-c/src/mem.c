#include "mem.h"
#include "bool.h"
#include "list.h"
#include "uart.h"
#include <stddef.h>

#define NOT_AVAILABLE -9999

static inline int buddy_idx(Frame *self) {
  int bit_to_invert = 1 << self->exp;
  return self->arr_index ^ bit_to_invert;
}

static bool provide_frame_with_exp(BuddyAllocater *alloc, int required_exp);

void buddy_dump(BuddyAllocater *alloc) {
  uart_println("========Status========");
  uart_println("Framenodes (idx, exp)");
  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    Frame *node = &alloc->frame_array[i];
    uart_printf("[%d,%d,%x]", node->arr_index, node->exp, node->addr);
  }
  uart_println("");

  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    list_head_t *list_head = &(alloc->free_lists[i]);
    struct list_head *entry;

    uart_printf("Freelist[%d]", i);
    for (entry = list_head->next; entry != list_head; entry = entry->next) {
      Frame *node = (Frame *)entry;
      uart_printf("->[%d,%d]", node->arr_index, node->exp);
    }
    uart_println("");
  }
  uart_println("======================");
}

struct Frame *buddy_alloc(BuddyAllocater *alloc, int target_exp) {
  if (target_exp >= BUDDY_MAX_EXPONENT) {
    return NULL;
  }
  bool success = provide_frame_with_exp(alloc, target_exp);
  if (success) {
    Frame *node = (Frame *)list_pop(&alloc->free_lists[target_exp]);
    return node;
  } else {
    return NULL;
  }
}
void buddy_free(BuddyAllocater *alloc, struct Frame *frame) {
  Frame *node, *buddy;
  Frame *low, *high;
  int frame_idx;
  for (frame_idx = frame->arr_index;;) {
    node = &alloc->frame_array[frame_idx];
    if (buddy_idx(node) >= BUDDY_NUM_FRAMES) {
      list_push(&node->list_base, &alloc->free_lists[node->exp]);
      uart_println(" push to freelist: node(idx:%d,exp:%d)", node->arr_index,
                   node->exp);
      break;
    }
    buddy = &alloc->frame_array[buddy_idx(node)];
    uart_printf("Try to merge buddy(idx:%d,exp:%d) node(idx:%d,exp:%d)",
                buddy->arr_index, buddy->exp, node->arr_index, node->exp);

    // Buddy is currently not in any list, therefore in used
    if (buddy->list_base.next == NULL) {
      list_push(&node->list_base, &alloc->free_lists[node->exp]);
      uart_println(" busy");
      uart_println(" push to freelist: node(idx:%d,exp:%d)", node->arr_index,
                   node->exp);
      break;
    }
    list_del(&buddy->list_base);

    // node in order
    low = node->arr_index < buddy->arr_index ? node : buddy;
    high = node->arr_index < buddy->arr_index ? buddy : node;

    high->exp = -1;
    low->exp += 1;
    frame_idx = low->arr_index;
    uart_println(" merged");
  }
}

bool provide_frame_with_exp(BuddyAllocater *alloc, int required_exp) {
  if (!list_empty(&alloc->free_lists[required_exp])) {
    return true;
  }

  for (int target = required_exp; target <= BUDDY_MAX_EXPONENT; target++) {
    // Find the first exp that alloc->free_lists[target] is not empty
    if (list_empty(&alloc->free_lists[target])) {
      continue;
    }

    // Split nodes from the top to bottom
    uart_puts("Split node from list(exp)");
    for (int exp = target; exp > required_exp; exp--) {
      uart_printf(" %d", exp);
      Frame *node = (Frame *)list_pop(&alloc->free_lists[exp]);

      int child_exp = exp - 1;
      Frame *child1 = &alloc->frame_array[node->arr_index];
      Frame *child2 = &alloc->frame_array[node->arr_index + (1 << child_exp)];
      child1->exp = child_exp;
      child2->exp = child_exp;
      list_push(&child1->list_base, &alloc->free_lists[child_exp]);
      list_push(&child2->list_base, &alloc->free_lists[child_exp]);
    }
    uart_println("");
    return true;
  }
  return false;
}

void buddy_init(BuddyAllocater *alloc, Frame *frame_arr) {

  // Bind the physical frame for manipulation
  alloc->frame_array = frame_arr;

  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    alloc->frame_array[i].arr_index = i;
    alloc->frame_array[i].exp = -1;
    alloc->frame_array[i].addr =
        (void *)(((long)i << BUDDY_FRAME_SHIFT) + MEMORY_START);
    alloc->frame_array[i].list_base.next = NULL;
    alloc->frame_array[i].list_base.prev = NULL;
  }
  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    list_init(&alloc->free_lists[i]);
  }
  // push a root frame
  Frame *root_frame = alloc->frame_array + 0;
  root_frame->exp = BUDDY_MAX_EXPONENT;
  list_push(&root_frame->list_base, &alloc->free_lists[BUDDY_MAX_EXPONENT]);
}

void KAllocManager_init() {
  AllocationManager *am = &KAllocManager;
  buddy_init(am->frame_allocator, Frames);
}

void KAllocManager_show_status() { buddy_dump(KAllocManager.frame_allocator); }

void *kalloc(int blocks) {
  // allcation using buddy system
  for (int i = 0; i < BUDDY_MAX_EXPONENT; i++) {
    if ((i << BUDDY_FRAME_SHIFT) > blocks) {
      uart_println("Allocate Request exp:%d", i);
      Frame *frame = buddy_alloc(KAllocManager.frame_allocator, i);
      uart_println("Allocated addr:%x, frame_idx:%d", frame->addr,
                   frame->arr_index);
      return frame->addr;
    }
  }
  return NULL;
}
void kfree(void *addr) {
  // Get frame from address provided
  int arr_index = (long)(addr - MEMORY_START) >> BUDDY_FRAME_SHIFT;
  Frame *frame = &Frames[arr_index];
  uart_println("Free Request addr:%x, frame_idx:%d", frame->addr,
               frame->arr_index);
  buddy_free(KAllocManager.frame_allocator, frame);
}
