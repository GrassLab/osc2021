#ifndef _PAGE_H_
#define _PAGE_H_
#include <types.h>

#define PD_INPUT_MASK 0xffff000000000000
#define PD_OUTPUT_MASK 0xfffffffffffff000
#define NUM_OF_ENTRY_PER_TABLE_4KB 512

#define pd_encode_addr(addr) (addr & PD_OUTPUT_MASK)
#define pd_decode_addr(addr) ( (addr & PD_OUTPUT_MASK) | PD_INPUT_MASK)

void* page_map_binary(void* addr, size_t size);
void page_map_stack(void* addr);
void* page_allocate(size_t* par_addr, int idx);
void* page_pgd_allocate();
void* get_physical_addr(void* addr, int i, int j, int k, int l);
size_t page_cal_total_page_entry(size_t total_low_level_entry, size_t table_size);
size_t page_get_num_of_table_entry(size_t *total_low_level_entry);
extern size_t pd_encode_table(void* addr);
extern size_t pd_encode_ram(void* addr);
extern size_t pd_encode_ram_block(void* addr);

#endif