#pragma once
#include "bool.h"
#include "cfg.h"
#include "test.h"

#define STARTUP_MAX_RESERVE_COUNT 10
// Area reserved by startup allocator
typedef struct ReservedArea {
  void *addr;
  unsigned long size;
} ReservedArea;

typedef struct StartupAllocator {
  int num_reserved;
  int max_reserved;
  struct ReservedArea *_reserved;
} StartupAllocator;

struct ReservedArea ReservedAreas[STARTUP_MAX_RESERVE_COUNT];

void StartupAllocator_init(StartupAllocator *sa, struct ReservedArea *reserved,
                           int max_reserved_count);

bool reserveMemory(StartupAllocator *sa, void *addr, unsigned long size);

void test_startup_alloc();