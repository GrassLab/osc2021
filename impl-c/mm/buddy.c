#include "bool.h"
#include "list.h"
#include "mm.h"
#include "mm/alloc.h"
#include "mm/startup.h"
#include "uart.h"
#include <stddef.h>

#define NOT_AVAILABLE -9999

static inline int buddy_idx(Frame *self) {
  int bit_to_invert = 1 << self->exp;
  return self->arr_index ^ bit_to_invert;
}

static inline void *end_addr(Frame *f) {
  void *addr = f->addr + ((1 << f->exp) << FRAME_SHIFT);
  return addr;
}

static bool provide_frame_with_exp(BuddyAllocater *alloc, int required_exp);
static Frame *find_buddy_collide_reserved(BuddyAllocater *alloc,
                                          StartupAllocator_t *sa);
static bool is_frame_wrapped_by_collison(Frame *node, StartupAllocator_t *sa);
static void buddy_init_reserved(BuddyAllocater *alloc, StartupAllocator_t *sa);

void buddy_dump(BuddyAllocater *alloc) {
  uart_println("========Status========");
  uart_println("Framenodes (idx, exp)");
  for (int i = 0; i < (1 << BUDDY_MAX_EXPONENT); i++) {
    Frame *node = &alloc->frames[i];
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
    node = &alloc->frames[frame_idx];
    if (buddy_idx(node) >= (1 << BUDDY_MAX_EXPONENT)) {
      list_push(&node->list_base, &alloc->free_lists[node->exp]);
      uart_println(" push to freelist: node(idx:%d,exp:%d)", node->arr_index,
                   node->exp);
      break;
    }
    buddy = &alloc->frames[buddy_idx(node)];
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
      Frame *child1 = &alloc->frames[node->arr_index];
      Frame *child2 = &alloc->frames[node->arr_index + (1 << child_exp)];
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

// Return a frame if it's area is collide with the reserved area
Frame *find_buddy_collide_reserved(BuddyAllocater *alloc,
                                   StartupAllocator_t *sa) {
  int total_reserved = sa->num_reserved;
  Frame *node;
  list_head_t *list, *entry;
  MemRegion node_region;
  bool overlap = false;
  // for every list
  for (int exp = 0; exp <= BUDDY_MAX_EXPONENT; exp++) {
    list = &alloc->free_lists[exp];
    // for every entries
    for (entry = list->next; entry != list; entry = entry->next) {
      node = (Frame *)entry;
      node_region.addr = node->addr;
      node_region.size = (1 << (node->exp)) << FRAME_SHIFT;
      for (int i = 0; i < total_reserved; i++) {
        overlap = is_overlap(&node_region, &sa->_reserved[i]);
        if (overlap) {
          return node;
        }
      }
    }
  }
  return NULL;
}

bool is_frame_wrapped_by_collison(Frame *node, StartupAllocator_t *sa) {
  int total_reserved = sa->num_reserved;
  // node region
  void *nstart = node->addr;
  void *nend = nstart + ((1 << (node->exp)) << FRAME_SHIFT);

  // reserved region
  void *rstart, *rend;

  for (int i = 0; i < total_reserved; i++) {
    rstart = sa->_reserved[i].addr;
    rend = rstart + sa->_reserved[i].size;
    if (rstart <= nstart && nend <= rend) {
      return true;
    }
  }
  return false;
}

// Initialize buddy systems initial state to prevent overlap with reserved
// memregion
void buddy_init_reserved(BuddyAllocater *alloc, StartupAllocator_t *sa) {
  Frame *node, *child1, *child2;
  int child_exp;

  MemRegion reg;
  for (int i = 0; i < sa->num_reserved; i++) {
    reg.addr = sa->_reserved[i].addr;
    reg.size = sa->_reserved[i].size;
    uart_println("reserved: %d, %d", reg.addr, reg.size);
  }
  while (NULL != (node = find_buddy_collide_reserved(alloc, sa))) {
    // split node
    list_del(&node->list_base);
    if (true == is_frame_wrapped_by_collison(node, sa)) {
      uart_println("collison -  remove node[%x, %x]", node->addr,
                   end_addr(node));
      continue;
    }

    uart_println("collison -  node[%x, %x]", node->addr, end_addr(node));
    child_exp = (node->exp) - 1;
    child1 = &alloc->frames[node->arr_index];
    child2 = &alloc->frames[node->arr_index + (1 << child_exp)];
    child1->exp = child_exp;
    child2->exp = child_exp;
    if (false == is_frame_wrapped_by_collison(child1, sa)) {
      uart_println("collison -  push child[%x, %x]", child1->addr,
                   end_addr(child1));
      list_push(&child1->list_base, &alloc->free_lists[child_exp]);
    }
    if (false == is_frame_wrapped_by_collison(child2, sa)) {
      uart_println("collison -  push child[%x, %x]", child2->addr,
                   end_addr(child2));
      list_push(&child2->list_base, &alloc->free_lists[child_exp]);
    }
    // buddy_dump(alloc);
  }
}

void buddy_init(BuddyAllocater *alloc, StartupAllocator_t *sa,
                struct Frame *frames) {
  alloc->frames = frames;
  for (int i = 0; i < (1 << BUDDY_MAX_EXPONENT); i++) {
    alloc->frames[i].arr_index = i;
    alloc->frames[i].exp = -1;
    alloc->frames[i].addr = (void *)(((long)i << FRAME_SHIFT) + MEMORY_START);
    alloc->frames[i].list_base.next = NULL;
    alloc->frames[i].list_base.prev = NULL;
  }
  for (int i = 0; i < BUDDY_NUM_FREE_LISTS; i++) {
    // uart_println("addr for list %d, %x", i, &(alloc->free_lists[i]));
    list_init(&alloc->free_lists[i]);
  }

  // push a root frame
  Frame *root_frame = &alloc->frames[0];
  root_frame->exp = BUDDY_MAX_EXPONENT;
  list_push(&root_frame->list_base, &alloc->free_lists[BUDDY_MAX_EXPONENT]);
  buddy_init_reserved(alloc, sa);
}
