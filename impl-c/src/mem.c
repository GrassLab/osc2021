#include "mem.h"
#include "bool.h"
#include "uart.h"
#include <stddef.h>

#define NOT_AVAILABLE -9999

static int _min_exp_satisfy(int request);

// FrameNode private functions
static int _buddy_idx(FrameNode *self);
static void _push_to_list(FrameNode *self, FrameNode *list);
static void _remove_from_list(FrameNode *self);

// FrameNode free-list private functions
static FrameNode *_list_pop(FrameNode *list);
static bool _list_is_empty(FrameNode *list);
static void _list_init(FrameNode *list);

// BuddyAllocater private function
static FrameNode *_buddy_list_for_exp(BuddyAllocater *self, int exp);

static bool _buddy_provide_frame_with_exp(BuddyAllocater *self,
                                          int required_exp);

void dump_buddy(BuddyAllocater *self) {
  uart_println("========Status========");
  uart_println("Framenodes (idx, exp)");
  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    FrameNode *node = &self->frame_array[i];
    uart_printf("[%d,%d]", node->arr_index, node->exp);
  }
  uart_println("");

  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    FrameNode *root = &(self->free_lists[i]);
    FrameNode *node = self->free_lists[i].next;

    uart_printf("Freelist[%d]", i);
    for (; node != root; node = node->next) {
      uart_printf("->[%d,%d]", node->arr_index, node->exp);
    }
    uart_println("");
  }
  uart_println("======================");
}

int _buddy_idx(FrameNode *self) {
  int bit_to_invert = 1 << self->exp;
  return self->arr_index ^ bit_to_invert;
}

void _push_to_list(FrameNode *self, FrameNode *list) {
  self->prev = list->prev;
  self->next = list;
  list->prev->next = self;
  list->prev = self;
}

void _remove_from_list(FrameNode *self) {
  FrameNode *prev = self->prev;
  FrameNode *next = self->next;
  prev->next = next;
  next->prev = prev;
}

FrameNode *_list_pop(FrameNode *list) {
  FrameNode *node = list->prev;
  _remove_from_list(node);
  return node;
}

bool _list_is_empty(FrameNode *list) { return list->next == list; }

void _list_init(FrameNode *list) {
  list->arr_index = NOT_AVAILABLE;
  list->exp = NOT_AVAILABLE;
  list->next = list;
  list->prev = list;
}
int _min_exp_satisfy(int request) {
  int n = 1;
  int exp = 0;
  while (n < request) {
    n = 1 << n;
    exp++;
  }
  return exp;
}

FrameNode *_buddy_list_for_exp(BuddyAllocater *self, int exp) {
  return &(self->free_lists[exp]);
}

int buddy_alloc(BuddyAllocater *self, int size_in_byte) {
  int num_frame_requested = (int)size_in_byte / BUDDY_FRAME_SIZE;
  int target_exp = _min_exp_satisfy(num_frame_requested);
  uart_println("Allocate Request size:%d, exp:%d", size_in_byte, target_exp);
  if (target_exp >= BUDDY_MAX_EXPONENT) {
    return -1;
  }
  bool success = _buddy_provide_frame_with_exp(self, target_exp);
  if (success) {
    FrameNode *node = _list_pop(_buddy_list_for_exp(self, target_exp));
    node->prev = NULL;
    node->next = NULL;

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
    if (_buddy_idx(node) >= BUDDY_NUM_FRAMES) {
      _push_to_list(node, _buddy_list_for_exp(self, node->exp));
      break;
    }
    buddy = &self->frame_array[_buddy_idx(node)];
    uart_printf("Try to merge  buddy(idx:%d,exp:%d) node(idx:%d,exp:%d)",
                buddy->arr_index, buddy->exp, node->arr_index, node->exp);

    // Buddy is currently not in any list, therefore in used
    if (buddy->next == NULL) {
      _push_to_list(node, _buddy_list_for_exp(self, node->exp));
      uart_println(" busy");
      break;
    }
    _remove_from_list(buddy);
    buddy->next = NULL;
    buddy->prev = NULL;

    // node in order
    low = node->arr_index < buddy->arr_index ? node : buddy;
    high = node->arr_index < buddy->arr_index ? buddy : node;

    high->exp = -1;
    low->exp += 1;
    frame_idx = low->arr_index;
    uart_println(" merged");
  }
}

bool _buddy_provide_frame_with_exp(BuddyAllocater *self, int required_exp) {
  if (!_list_is_empty(_buddy_list_for_exp(self, required_exp))) {
    return true;
  }

  // find upmost exp to split
  int target = required_exp;
  bool found = false;
  for (; target <= BUDDY_MAX_EXPONENT; target++) {
    if (!_list_is_empty(_buddy_list_for_exp(self, target))) {
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
    FrameNode *node = _list_pop(_buddy_list_for_exp(self, exp));

    int child_exp = exp - 1;
    FrameNode *child1 = &self->frame_array[node->arr_index];
    FrameNode *child2 = &self->frame_array[node->arr_index + (1 << child_exp)];
    child1->exp = child_exp;
    child2->exp = child_exp;
    _push_to_list(child1, _buddy_list_for_exp(self, child_exp));
    _push_to_list(child2, _buddy_list_for_exp(self, child_exp));
  }
  uart_println("");
  return true;
}

void buddy_init(BuddyAllocater *self) {
  for (int i = 0; i < BUDDY_NUM_FRAMES; i++) {
    self->frame_array[i].arr_index = i;
    self->frame_array[i].exp = -1;
    self->frame_array[i].next = NULL;
    self->frame_array[i].prev = NULL;
  }
  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    _list_init(&self->free_lists[i]);
  }
  // push a root frame
  FrameNode *root_frame = self->frame_array + 0;
  root_frame->exp = BUDDY_MAX_EXPONENT;
  _push_to_list(root_frame, _buddy_list_for_exp(self, BUDDY_MAX_EXPONENT));
}
