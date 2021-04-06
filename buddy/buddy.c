
#include "buddy/buddy.h"

#include <stdbool.h>
#include <stdint.h>

#include "printf/printf.h"

int buddy_table[BUDDY_MAX + 1][BUDDY_CNT];

void buddy_init() {
  for (int i = 0; i < BUDDY_MAX; i++) {
    buddy_table[i][0] = -1;
  }
  buddy_table[BUDDY_MAX][0] = 0;
  buddy_table[BUDDY_MAX][1] = 256;
  buddy_table[BUDDY_MAX][2] = 512;
  buddy_table[BUDDY_MAX][3] = -1;
}

void buddy_push(int index, int level) {
  int i = 0;
  while (i < BUDDY_CNT && buddy_table[level][i] >= 0) {
    i += 1;
  }
  if (buddy_table[level][i] >= BUDDY_CNT - 1) {
    return;
  }
  buddy_table[level][i] = index;
  buddy_table[level][i + 1] = -1;
  //
  printf("buddy push %d-%d\n", index, index + (1 << level));
}

int buddy_pop(int level) {
  if (buddy_table[level][0] < 0) {
    return -1;
  }
  // iterate through the level
  int i = 0;
  while (buddy_table[level][i + 1] >= 0) {
    i += 1;
  }
  int index = buddy_table[level][i];
  buddy_table[level][i] = -1;
  //
  printf("buddy pop %d-%d\n", index, index + (1 << level));
  return index;
}

void buddy_update() {
  int level = 0;
  while (level < BUDDY_MAX) {
    bool merged = true;
    while (merged) {
      merged = false;
      // find buddy blocks
      int i = 0;
      while (!merged && buddy_table[level][i] >= 0) {
        int j = i + 1;
        while (!merged && buddy_table[level][j] > 0) {
          if (buddy_table[level][j] == (buddy_table[level][i] ^ (1 << level))) {
            // merge buddy blocks
            int index = buddy_table[level][i] <= buddy_table[level][j]
                            ? buddy_table[level][i]
                            : buddy_table[level][j];
            printf("buddy pop %d-%d\n", buddy_table[level][j],
                   buddy_table[level][j] + (1 << level));
            while (buddy_table[level][j] > 0) {
              buddy_table[level][j] = buddy_table[level][j + 1];
              j += 1;
            }
            printf("buddy pop %d-%d\n", buddy_table[level][i],
                   buddy_table[level][i] + (1 << level));
            while (buddy_table[level][i] > 0) {
              buddy_table[level][i] = buddy_table[level][i + 1];
              i += 1;
            }
            buddy_push(index, level + 1);
            merged = true;
          }
          j += 1;
        }
        i += 1;
      }
    }
    level += 1;
  }
}

buddy_t buddy_alloc(int level) {
  printf("buddy alloc %d\n", level);
  buddy_t failed = {-1, -1};
  // get a feasible buddy block
  int current_level = level;
  while (current_level <= BUDDY_MAX && buddy_table[current_level][0] < 0) {
    current_level += 1;
  }
  if (buddy_table[current_level][0] < 0) {
    return failed;
  }
  int index = buddy_pop(current_level);
  if (index < 0) {
    return failed;
  }
  // split the buddy block if it is too big
  while (current_level > level) {
    current_level -= 1;
    buddy_push(index + (1 << current_level), current_level);
  }
  buddy_update();
  // setup return value
  buddy_t block = {index, current_level};
  return block;
}

void buddy_free(buddy_t block) {
  printf("buddy free %d\n", block.index);
  buddy_push(block.index, block.level);
  buddy_update();
}
