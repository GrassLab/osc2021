
#include "cfg.h"
#include "test.h"
#include "uart.h"

#include "mm/frame.h"
#include "mm/startup.h"

#include <stddef.h>

StartupAllocator_t StartupAlloc;
struct MemRegion ReservedRegions[STARTUP_MAX_RESERVE_COUNT];

// Mask for address inside frame
#define FRAME_MASK ((1 << FRAME_SHIFT) - 1)

static void sa_init(StartupAllocator_t *sa, struct MemRegion *reserved,
                    int max_reserved_count);
// static void *sa_alloc(StartupAllocator_t *sa, unsigned long size);
static bool sa_reserve(StartupAllocator_t *sa, void *addr, unsigned long size);

void startup_init() {
  sa_init(&StartupAlloc, ReservedRegions, STARTUP_MAX_RESERVE_COUNT);
}
bool startup_reserve(void *addr, unsigned long size) {
  bool succes = sa_reserve(&StartupAlloc, addr, size);
  if (succes) {
    uart_println("reserve addr: %x", addr);
  } else {
    uart_println("failed to reserve addr: %x", addr);
  }
  return succes;
}

// void *startup_alloc(unsigned long size) {
//   return sa_alloc(&StartupAlloc, size);
// }

// If two meory region is overlap
bool is_overlap(MemRegion *a1, MemRegion *a2) {
  MemRegion *low, *high;
  low = a1->addr < a2->addr ? a1 : a2;
  high = a1->addr < a2->addr ? a2 : a1;
  unsigned long low_end = (unsigned long)low->addr + low->size;
  return low_end > ((unsigned long)high->addr);
}

// Makesure MemoryRegions is sorted
void sa_sort_reserved(StartupAllocator_t *sa) {
  // simple bubble sort
  MemRegion tmpArea;
  for (int i = 0; i < sa->num_reserved; i++) {
    for (int j = i + 1; j < sa->num_reserved; j++) {
      if (sa->_reserved[i].addr < sa->_reserved[j].addr) {
        tmpArea = sa->_reserved[i];
        sa->_reserved[i] = sa->_reserved[j];
        sa->_reserved[j] = tmpArea;
      }
    }
  }
}

// Init a StarupAllocator struct
void sa_init(StartupAllocator_t *sa, struct MemRegion *reserved,
             int max_reserved_count) {
  sa->max_reserved = max_reserved_count;
  sa->num_reserved = 0;
  sa->_reserved = reserved;
  for (int i = 0; i < sa->max_reserved; i++) {
    sa->_reserved[i].addr = 0;
    sa->_reserved[i].size = 0;
  }
}

// void *sa_alloc(StartupAllocator_t *sa, unsigned long size) {

//   return NULL;
// }

// Reserve a memory region
bool sa_reserve(StartupAllocator_t *sa, void *addr, unsigned long size) {
  if (((unsigned long long)addr & FRAME_MASK) != 0 ||
      (size & FRAME_MASK) != 0) {
    uart_println("not aligned");
    return false;
  }
  if (sa->num_reserved >= sa->max_reserved) {
    uart_println("limited reached");
    return false;
  }
  MemRegion requested = {.addr = addr, .size = size};
  // Should not overlapped with space that reserved already
  for (int i = 0; i < sa->num_reserved; i++) {
    if (is_overlap(&requested, &sa->_reserved[i])) {
      uart_println("overlap");
      return false;
    }
  }
  sa->_reserved[sa->num_reserved] = requested;
  sa->num_reserved += 1;
  sa_sort_reserved(sa);
  return true;
}

//============== TEST START =================
#ifdef CFG_RUN_STATUP_ALLOC_TEST

bool test_is_overlap() {
  MemRegion a1 = {.addr = (void *)0, .size = 13};
  MemRegion a2 = {.addr = (void *)20, .size = 5};
  uart_println("case: not overlap");
  assert(is_overlap(&a1, &a2) == false);

  MemRegion a3 = {.addr = (void *)0, .size = 13};
  MemRegion a4 = {.addr = (void *)2, .size = 5};
  uart_println("case: range include");
  assert(is_overlap(&a3, &a4) == true);

  MemRegion a5 = {.addr = (void *)0, .size = 13};
  MemRegion a6 = {.addr = (void *)2, .size = 5};
  uart_println("case: partial overlap");
  assert(is_overlap(&a5, &a6) == true);
  return true;
};

bool test_sa_sort_reserved() {
  StartupAllocator_t sa;
  struct MemRegion reservd[5];
  struct MemRegion a1 = {.addr = (void *)4, .size = 2};
  struct MemRegion a2 = {.addr = (void *)99, .size = 2};
  sa_init(&sa, reservd, 5);
  sa._reserved[0] = a1;
  sa._reserved[1] = a2;
  sa.num_reserved = 2;
  sa_sort_reserved(&sa);
  assert(sa._reserved[0].addr == a2.addr);
  assert(sa._reserved[0].size == a2.size);
  assert(sa._reserved[1].addr == a1.addr);
  assert(sa._reserved[1].size == a1.size);
  return true;
};

bool test_sa_init() {
  StartupAllocator_t sa;
  struct MemRegion reservd[5];
  reservd[3].size = 1 << 3;
  sa_init(&sa, reservd, 5);
  assert(sa.max_reserved == 5);
  assert(sa.num_reserved == 0);
  assert(reservd[3].size == 0);
  return true;
}

bool test_sa_reserve() {
  StartupAllocator_t sa;
  struct MemRegion reservd[5];
  sa_init(&sa, reservd, 5);

  void *start = (void *)(2 * FRAME_ADDR_BASE);
  unsigned long size = (3 * FRAME_ADDR_BASE);
  sa_reserve(&sa, start, size);
  assert(sa.num_reserved == 1);
  assert(sa._reserved[0].addr == start);

  void *not_aligned_start = (void *)(2 * FRAME_ADDR_BASE - 1);
  unsigned long not_aligned_size = size - 87;
  uart_println("case: start addr not page aligned");
  assert(sa_reserve(&sa, not_aligned_start, size) == false);
  uart_println("case: reserve size not page aligned");
  assert(sa_reserve(&sa, start, not_aligned_size) == false);

  void *overlap_start = (void *)(2 * FRAME_ADDR_BASE);
  uart_println("case: overlap mem");
  assert(sa_reserve(&sa, overlap_start, size) == false);

  return true;
}
#endif

void test_startup_alloc() {
#ifdef CFG_RUN_STATUP_ALLOC_TEST
  unittest(test_is_overlap, "starup_alloc", "is_overlap");
  unittest(test_sa_init, "starup_alloc", "init");
  unittest(test_sa_reserve, "starup_alloc", "sa_reserve");
  unittest(test_sa_sort_reserved, "starup_alloc", "sort_reserve");
#endif
}