#pragma once
#include "bool.h"
#include "cfg.h"
#include "test.h"

#define STARTUP_MAX_RESERVE_COUNT 10

typedef struct MemRegion {
  void *addr;
  unsigned long size;
} MemRegion;

typedef struct StartupAllocator {
  int num_reserved;
  int max_reserved;
  struct MemRegion *_reserved;
} StartupAllocator_t;

extern struct MemRegion ReservedRegions[STARTUP_MAX_RESERVE_COUNT];
extern StartupAllocator_t StartupAlloc;

bool is_overlap(MemRegion *a1, MemRegion *a2);

// Public api: Initialize the starup allocator module
void startup_init();

// Request StartupAllocator to allocate a space
// void *startup_alloc(unsigned long size);

// Reqeust StartupAllocator to reserve an area
bool startup_reserve(void *addr, unsigned long size);

// Only used for running tests
void test_startup_alloc();