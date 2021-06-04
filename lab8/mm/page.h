#ifndef _PAGE_H_
#define _PAGE_H_
#include <types.h>

#define NUM_OF_ENTRY_PER_TABLE_4KB 512

void* page_alloc(void* addr, size_t size);
int map_phys_to_virt(void* addr, size_t size);
void* get_physical_addr(void* addr, int i, int j, int k, int l);
extern size_t pd_encode_table(void* addr);
extern size_t pd_encode_ram(void* addr);
extern size_t pd_encode_ram_block(void* addr);
#endif