#ifndef	_MM_H
#define	_MM_H
#include "peripherals/base.h"
#include "sched.h"
#define VA_START 			    0xffff000000000000
#define PAGE_MASK			    0xfffffffffffff000
#define LOCAL_PHY               0x80000000
#define PHYS_MEMORY_SIZE 		0x40000000	
#define USER_TEXT_OFFSET        0x80000
#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define MAX_ORDER               10 
#define MAX_PAGE                1024
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)
#define SECTION_SIZE			(1 << SECTION_SHIFT)
#define LOW_MEMORY              (2 * SECTION_SIZE)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define TWO_PAGE_SIZE   		((1 << PAGE_SHIFT) * 2)	
#define PAGING_PAGES 			1024

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)
#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT
#define PG_DIR_SIZE			(3 * PAGE_SIZE)
#define IDP_DIR_SIZE        (4 * PAGE_SIZE)


#ifndef __ASSEMBLER__
#include "sched.h"
// for debug
void print_all_buddy();

// allocate block from buddy system and return physical address of first page from given size
struct block *alloc_block(int num_page);

// free block from buddy system and merge the blocks
void put_back_block(struct block *free_block);

// pfn to page physical address
unsigned long pfn_to_phy(int pfn);

// physical address to virtual address

// init buddy system
void init_buddy_system();

// buddy system
extern struct free_area buddy_entry[MAX_ORDER];

struct block *get_free_page();
void free_user_page(struct task_struct *task);
void free_kernel_page(struct task_struct *task);
void map_page(struct task_struct *task, unsigned long va, struct block *alloc, unsigned long prot);
void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long src, unsigned long dst, unsigned long n);

int copy_virt_memory(struct task_struct *dst);
struct block *allocate_kernel_page();
unsigned long allocate_user_page(struct task_struct *task, unsigned long va, unsigned long prot, int num_pages);
int get_remain_num();

extern unsigned long pg_dir;
#endif
#endif