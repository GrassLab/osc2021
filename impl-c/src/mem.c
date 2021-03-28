#include "mem.h"
#include "bool.h"
#include "list.h"
#include "uart.h"
#include <stddef.h>

#define NOT_AVAILABLE -9999

static inline int buddy_idx(FrameNode *self) {
  int bit_to_invert = 1 << self->exp;
  return self->arr_index ^ bit_to_invert;
}

static int min_exp_satisfy(int request) {
  int n = 1;
  int exp = 0;
  while (n < request) {
    n = 1 << n;
    exp++;
  }
  return exp;
}

// BuddyAllocater private function

static bool provide_frame_with_exp(BuddyAllocater *self, int required_exp);

void dump_buddy(BuddyAllocater *self) {
  uart_println("========Status========");
  uart_println("Framenodes (idx, exp)");
  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    FrameNode *node = &self->frame_array[i];
    uart_printf("[%d,%d]", node->arr_index, node->exp);
  }
  uart_println("");

  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    list_head_t *list_head = &(self->free_lists[i]);
    struct list_head *entry;

    uart_printf("Freelist[%d]", i);
    for (entry = list_head->next; entry != list_head; entry = entry->next) {
      FrameNode *node = (FrameNode *)entry;
      uart_printf("->[%d,%d]", node->arr_index, node->exp);
    }
    uart_println("");
  }
  uart_println("======================");
}

int buddy_alloc(BuddyAllocater *self, int size_in_byte) {
  int num_frame_requested = (int)size_in_byte / BUDDY_FRAME_SIZE;
  int target_exp = min_exp_satisfy(num_frame_requested);
  uart_println("Allocate Request size:%d, exp:%d", size_in_byte, target_exp);
  if (target_exp >= BUDDY_MAX_EXPONENT) {
    return -1;
  }
  bool success = provide_frame_with_exp(self, target_exp);
  if (success) {
    FrameNode *node = (FrameNode *)list_pop(&self->free_lists[target_exp]);

    // TODO: remove mutiplication
    return node->arr_index * BUDDY_FRAME_SIZE;
  } else {
    return -1;
  }
}
void buddy_free(BuddyAllocater *self, int addr) {
  int frame_idx = addr / BUDDY_FRAME_SIZE;
  uart_println("Free Request addr:%d, frame_idx:%d", addr, frame_idx);

  FrameNode *node, *buddy;
  FrameNode *low, *high;
  for (;;) {
    node = &self->frame_array[frame_idx];
    if (buddy_idx(node) >= BUDDY_NUM_FRAMES) {
      list_push(&node->list_base, &self->free_lists[node->exp]);
      break;
    }
    buddy = &self->frame_array[buddy_idx(node)];
    uart_printf("Try to merge  buddy(idx:%d,exp:%d) node(idx:%d,exp:%d)",
                buddy->arr_index, buddy->exp, node->arr_index, node->exp);

    // Buddy is currently not in any list, therefore in used
    if (buddy->list_base.next == NULL) {
      list_push(&node->list_base, &self->free_lists[node->exp]);
      uart_println(" busy");
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

bool provide_frame_with_exp(BuddyAllocater *self, int required_exp) {
  if (!list_empty(&self->free_lists[required_exp])) {
    return true;
  }

  // find upmost exp to split
  int target = required_exp;
  bool found = false;
  for (; target <= BUDDY_MAX_EXPONENT; target++) {
    if (!list_empty(&self->free_lists[target])) {
      found = true;
      break;
    }
  }
  if (found == false) {
    // no free list available
    return false;
  }

  int upmost_exp = target;
  uart_puts("Split node from list(exp)");
  for (int exp = upmost_exp; exp > required_exp; exp--) {
    uart_printf(" %d", exp);
    FrameNode *node = (FrameNode *)list_pop(&self->free_lists[exp]);

    int child_exp = exp - 1;
    FrameNode *child1 = &self->frame_array[node->arr_index];
    FrameNode *child2 = &self->frame_array[node->arr_index + (1 << child_exp)];
    child1->exp = child_exp;
    child2->exp = child_exp;
    list_push(&child1->list_base, &self->free_lists[child_exp]);
    list_push(&child2->list_base, &self->free_lists[child_exp]);
  }
  uart_println("");
  return true;
}

void buddy_init(BuddyAllocater *self) {
  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    self->frame_array[i].arr_index = i;
    self->frame_array[i].exp = -1;
    self->frame_array[i].list_base.next = NULL;
    self->frame_array[i].list_base.prev = NULL;
  }
  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    list_init(&self->free_lists[i]);
  }
  // push a root frame
  FrameNode *root_frame = self->frame_array + 0;
  root_frame->exp = BUDDY_MAX_EXPONENT;
  list_push(&root_frame->list_base, &self->free_lists[BUDDY_MAX_EXPONENT]);
}
