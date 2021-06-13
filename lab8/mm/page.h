#ifndef _PAGE_H_
#define _PAGE_H_
#include <types.h>

#define PD_INPUT_MASK 0xffff000000000000
#define PD_INPUT_OFFSET_MASK 0xffffffffffffffff
#define PD_OUTPUT_MASK 0x0000fffffffff000
#define PD_OUTPUT_OFFSET_MASK 0x0000ffffffffffff
#define NUM_OF_ENTRY_PER_TABLE_4KB 512

//put physical address to page table, align to 0x1000
#define pd_encode_addr(addr) (addr & PD_OUTPUT_MASK)

//put physical address to page table with offset
#define pd_encode_offset(addr) (addr & PD_OUTPUT_OFFSET_MASK)

//read physical address from page table, and translate to 0xffffxxxxxxxxxxxx range
#define pd_decode_addr(addr) ((addr & PD_OUTPUT_MASK) | PD_INPUT_MASK)

void* page_map_binary(void* addr, size_t size, size_t* ctx_pgd);
void page_map_stack(void* addr, size_t* ctx_pgd);
void* page_allocate(size_t* par_addr, int idx);
void* page_pgd_allocate(size_t* ctx_pgd);
void* get_physical_addr(void* addr, int i, int j, int k, int l);
size_t page_cal_total_page_entry(size_t total_low_level_entry, size_t table_size);
size_t page_get_num_of_table_entry(size_t *total_low_level_entry);
extern size_t pd_encode_table(void* addr);
extern size_t pd_encode_ram(void* addr);
extern size_t pd_encode_ram_block(void* addr);
void page_free(size_t* ctx_pgd, size_t size);
#endif