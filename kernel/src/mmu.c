#include "mmu.h"

#include <stdint.h>

#include "alloc.h"
#include "mmu_values.h"
#include "printf.h"
#include "thread.h"

void init_page_table(uint64_t **table) {
  *table = (uint64_t *)thread_allocate_page(get_current(), PAGE_SIZE);
  for (int i = 0; i < 512; i++) {
    *((*table) + i) = 0;
  }
  // printf("[init] next table virtual addr: 0x%llx\n", (uint64_t)(*table));
  *table = (uint64_t *)VA2PA(*table);
}

void update_page_table(uint64_t *pgd, uint64_t virtual_addr,
                       uint64_t physical_addr, int permission) {
  if (pgd == 0) {
    printf("Invalid PGD!!\n");
    return;
  }

  uint32_t index[4] = {
      (virtual_addr >> 39) & 0x1ff, (virtual_addr >> 30) & 0x1ff,
      (virtual_addr >> 21) & 0x1ff, (virtual_addr >> 12) & 0x1ff};

  // printf("virtual addr: 0x%llx", virtual_addr);
  // printf(", index: 0x%llx", index[0]);
  // printf(", index: 0x%llx", index[1]);
  // printf(", index: 0x%llx", index[2]);
  // printf(", index: 0x%llx\n", index[3]);
  // printf("physical addr: 0x%llx\n", physical_addr);

  uint64_t *table = (uint64_t *)PA2VA(pgd);
  // printf("table: 0x%llx\n", (uint64_t)table);
  for (int level = 0; level < 3; level++) {
    if (table[index[level]] == 0) {
      // printf("level: %d, index: 0x%llx  ", level, index[level]);
      init_page_table((uint64_t **)&(table[index[level]]));
      table[index[level]] |= PD_TABLE;
    }
    // printf("table PA: 0x%llx\n", (uint64_t)table[index[level]]);
    table = (uint64_t *)PA2VA(table[index[level]] & ~0xfff);
    // printf("table VA: 0x%llx\n", (uint64_t)table);
  }
  uint64_t BOOT_RWX_ATTR = (1 << 6);
  if (permission & 0b010)
    BOOT_RWX_ATTR |= 0;
  else
    BOOT_RWX_ATTR |= (1 << 7);
  // printf("0x%llx\n", BOOT_RWX_ATTR);
  table[index[3]] =
      physical_addr | BOOT_PTE_NORMAL_NOCACHE_ATTR | BOOT_RWX_ATTR;
  // printf("page PA: 0x%llx\n", (uint64_t)table[index[3]]);
}
