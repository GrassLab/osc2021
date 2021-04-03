#include "cfg.h"
#include "mm/frame.h"
#include "mm/startAlloc.h"
#include "test.h"
#include "uart.h"

// Mask for address inside frame
#define FRAME_MASK ((1 << FRAME_SHIFT) - 1)

bool is_overlap(ReservedArea *a1, ReservedArea *a2);

bool is_overlap(ReservedArea *a1, ReservedArea *a2) {
  ReservedArea *low, *high;
  low = a1->addr < a2->addr ? a1 : a2;
  high = a1->addr < a2->addr ? a2 : a1;
  unsigned long low_end = (unsigned long)low->addr + low->size;
  return low_end > ((unsigned long)high->addr);
}

void sort_reserved(StartupAllocator *sa) {
  // simple bubble sort
  for (int i = 0; i < sa->max_reserved; i++) {
    for (int j = i + 1; j < sa->max_reserved; j++) {
      int k = 0;
      k++;
    }
  }
}

void StartupAllocator_init(StartupAllocator *sa, struct ReservedArea *reserved,
                           int max_reserved_count) {
  sa->max_reserved = max_reserved_count;
  sa->num_reserved = 0;
  sa->_reserved = reserved;
  for (int i = 0; i < sa->max_reserved; i++) {
    sa->_reserved[i].addr = 0;
    sa->_reserved[i].size = 0;
  }
}

bool reserveMemory(StartupAllocator *sa, void *addr, unsigned long size) {
  if (((unsigned long long)addr & FRAME_MASK) != 0 ||
      (size & FRAME_MASK) != 0) {
    return false;
  }
  if (sa->num_reserved >= sa->max_reserved) {
    return false;
  }
  ReservedArea requested = {.addr = addr, .size = size};
  // Should not overlapped with space that reserved already
  for (int i = 0; i < sa->num_reserved; i++) {
    if (is_overlap(&requested, &sa->_reserved[i])) {
      return false;
    }
  }
  sa->_reserved[sa->num_reserved] = requested;
  sa->num_reserved += 1;
  return true;
}

#ifdef CFG_RUN_STATUP_ALLOC_TEST
bool test_is_overlap() {
  ReservedArea a1 = {.addr = (void *)0, .size = 13};
  ReservedArea a2 = {.addr = (void *)20, .size = 5};
  uart_println("case: not overlap");
  assert(is_overlap(&a1, &a2) == false);

  ReservedArea a3 = {.addr = (void *)0, .size = 13};
  ReservedArea a4 = {.addr = (void *)2, .size = 5};
  uart_println("case: range include");
  assert(is_overlap(&a3, &a4) == true);

  ReservedArea a5 = {.addr = (void *)0, .size = 13};
  ReservedArea a6 = {.addr = (void *)2, .size = 5};
  uart_println("case: partial overlap");
  assert(is_overlap(&a5, &a6) == true);
  return true;
};

bool test_startup_alloc_init() {
  StartupAllocator a;
  struct ReservedArea reservd[5];
  reservd[3].size = 1 << 3;
  StartupAllocator_init(&a, reservd, 5);
  assert(a.max_reserved == 5);
  assert(a.num_reserved == 0);
  assert(reservd[3].size == 0);
  return true;
}

bool test_reserveMemory() {
  StartupAllocator a;
  struct ReservedArea reservd[5];
  StartupAllocator_init(&a, reservd, 5);

  void *start = (void *)(2 * FRAME_ADDR_BASE);
  unsigned long size = (3 * FRAME_ADDR_BASE);
  reserveMemory(&a, start, size);
  assert(a.num_reserved == 1);
  assert(a._reserved[0].addr == start);

  void *not_aligned_start = (void *)(2 * FRAME_ADDR_BASE - 1);
  unsigned long not_aligned_size = size - 87;
  uart_println("case: start addr not page aligned");
  assert(reserveMemory(&a, not_aligned_start, size) == false);
  uart_println("case: reserve size not page aligned");
  assert(reserveMemory(&a, start, not_aligned_size) == false);

  void *overlap_start = (void *)(2 * FRAME_ADDR_BASE);
  uart_println("case: overlap mem");
  assert(reserveMemory(&a, overlap_start, size) == false);

  return true;
}
#endif

void test_startup_alloc() {
#ifdef CFG_RUN_STATUP_ALLOC_TEST
  unittest(test_is_overlap, "starup_alloc", "is_overlap");
  unittest(test_startup_alloc_init, "starup_alloc", "init");
  unittest(test_reserveMemory, "starup_alloc", "reserveMemory");
#endif
}