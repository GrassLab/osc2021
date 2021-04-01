#include "memory/memory.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "buddy/buddy.h"
#include "printf/printf.h"

uint64_t memory_table[MEMORY_MAX][MEMORY_CNT];
int memory_level[BUDDY_MAX];

void memory_init() {
  for (int i = 0; i < MEMORY_MAX; i++) {
    memory_table[i][0] = UINT64_MAX;
  }
}

void* memory_alloc(int size) {
  printf("memory alloc %d\n", size);
  if (size > BUDDY_SIZE) {
    return NULL;
  }
  int level = 0;
  while ((1 << level) * MEMORY_SIZE < size) {
    level += 1;
  }
  //
  if (memory_table[level][0] == UINT64_MAX) {
    buddy_t block = buddy_alloc(0);
    int memory_cnt = 1 << (BUDDY_EXP - MEMORY_EXP - level);
    for (int i = 0; i < memory_cnt; i++) {
      int64_t address = BUDDY_BASE + BUDDY_SIZE * block.index +
                        (1 << (MEMORY_EXP + level)) * i;
      // printf("memory push %d %p\n", level, address);
      memory_table[level][i] = address;
    }
    memory_table[level][memory_cnt] = UINT64_MAX;
    memory_level[block.index] = level;
  }
  //
  int i = 0;
  while (memory_table[level][i + 1] != UINT64_MAX) {
    i += 1;
  }
  uint64_t address = memory_table[level][i];
  memory_table[level][i] = UINT64_MAX;
  printf("memory pop %d %p\n", level, address);
  return (void*)address;
}

void memory_free(void* ptr) {
  printf("memory free %p\n", ptr);
  int level = memory_level[((uint64_t)ptr - BUDDY_BASE) >> BUDDY_EXP];
  int i = 0;
  while (memory_table[level][i] != UINT64_MAX) {
    i += 1;
  }
  memory_table[level][i] = (uint64_t)ptr;
  memory_table[level][i + 1] = UINT64_MAX;
  printf("memory push %d %p\n", level, ptr);
}
