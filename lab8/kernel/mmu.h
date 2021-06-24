#ifndef MMU_H
#define MMU_H

#include "thread.h"

#define KVA 0xffff000000000000

#define VA2PA(addr) ((unsigned long)(addr) & (unsigned long)0x0000ffffffffffff)
#define PA2VA(addr) ((unsigned long)(addr) | (unsigned long)0xffff000000000000)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_PAGE 0b11
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR (PD_TABLE)
#define BOOT_PUD_ATTR (PD_TABLE)
#define BOOT_PMD_ATTR (PD_TABLE)
#define BOOT_PTE_DEVICE_nGnRnE_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
#define BOOT_PTE_NORMAL_NOCACHE_ATTR (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

void init_page_table(struct thread *thread, unsigned long **table);
void update_page_table(struct thread *thread, unsigned long virtual_addr, unsigned long physical_addr, int permission);

#endif