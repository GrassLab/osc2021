#ifndef _MM_H
#define _MM_H

#include "base.h"

#define VA_START                   0xFFFF000000000000       // Virtual Memory Start
#define PHYS_MEMORY_SIZE 		   0x40000000               // 1G

#define PAGE_MASK                  0xFFFFFFFFFFFFF000
#define PAGE_SHIFT                 12
#define TABLE_SHIFT                9
#define SECTION_SHIFT              (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE                  (1 << PAGE_SHIFT)        // 4KB per page frame
#define SECTION_SIZE               (1 << SECTION_SHIFT)	
#define PAGE_FRMAME_NUM            4096

#define MAX_ORDER                  9 
#define MAX_ORDER_SIZE             (1 << MAX_ORDER)

#define LOW_MEMORY                 SECTION_SIZE             // 2MB
#define HIGH_MEMORY                DEVICE_BASE

#define PTRS_PER_TABLE			    (1 << TABLE_SHIFT)
#define PGD_SHIFT			        PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			        PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			        PAGE_SHIFT + TABLE_SHIFT
#define PG_DIR_SIZE                 (3 * PAGE_SIZE)         // Page dir size (PGD, PUD, PMD) 

#define FIND_BUDDY_PFN(pfn, order) ((pfn) ^ (1<<(order)))
#define FIND_LBUDDY_PFN(pfn, order)((pfn) & (~(1<<(order))))

#define MAX_OBJ_ALLOCTOR_NUM        16
#define MIN_ALLOCATAED_OBJ_SIZE     8                       // At least 8 bytes to store adress(address of next free object )
#define MAX_ALLOCATAED_OBJ_SIZE     2048                    // At most 2048 bytes (half of a page frame)

#define MIN_KMALLOC_ORDER           3
#define MAX_KMALLOC_ORDER           11

#define PFN_MASK                    0x0000FFFFFFFFF000
#define PHY_ADDR_TO_PFN(addr)       (((((unsigned long)(addr)) - LOW_MEMORY) & PFN_MASK) >> PAGE_SHIFT)


#ifndef __ASSEMBLER__

#include "list.h"
#include "types.h"
#include "sched.h"
/**
 * "used" property of struct of page 
 */
enum booking_status {
    Free, 
    Taken
};

typedef struct free_area_struct {
    int nr_free;
    struct list_head freelist;    
} free_area_t;

/**
 * 
 * @obj_used: The number of objects are used in this page
 * @obj_alloc: Belong to which object allocator
 * @free: Point to address of first free object. And it only use free->next
 *        (first 8 bytes) to get address of next free object
 */
typedef struct page {
    struct list_head list;
    
    int order;
    int pfn; // page frame number
    int used;
    uint64_t phy_addr;

    /* For Object allocator */
    int obj_used;   
    struct object_allocator *obj_alloc; 
    struct list_head *free; 
} page_t;

/**
 *  Initalization for Buddy system
 */
void page_init();
void free_area_init();
void dump_buddy();

/**
 * buddy_block_alloc - Allocate page frames in specify page order
 * @order: The Requested size of 2^order of page frames. 
 */
struct page *buddy_block_alloc(int order);

/**
 * buddy_block_free - Free page frames 
 * @order: Start address of page frames
 */
void buddy_block_free(struct page* block);
/**
 * push and pop block from free area and set some page configs.
 */
void push_block_to_free_area(page_t *, free_area_t *, int order);
void pop_block_from_free_area(page_t *, free_area_t *);

/**
 * Object Allocator - Allocate memory space to object.
 * Object allocator is based on Buudy system that are used for 
 * small object
 * @full: All objects in use
 * @partial: Has free objects in it and so is a prime candidate for allocation of objects.
 * @empty: Has no allocated objects and so is a prime candidate for slab destruction.
 * @curr_page: Point to page that used to allocate memory currently
 * To speed allocation and freeing of objects they are arranged 
 * into three lists; full, partial and empty. 
 *
 * @objsize: Memory size allocated by the allocator once 
 * @obj_per_page: The maximum number of obj in one page depends on objsize
 * @obj_used: The number of objects are using in all page
 * @page_used: The number of page are used in this allocator
 * 
 */
typedef struct object_allocator {
    struct list_head full;
    struct list_head partial;
    struct list_head empty;
    struct page *curr_page;

    int objsize;        
    int obj_per_page;   
    int obj_used;      
    int page_used;  
} obj_allocator_t;

/**
 * Initalize all members for Object allocator
 */
void __init_obj_alloc(obj_allocator_t *, int);
void __init_obj_page(page_t *);

/**
 * Register a new obj allocator
 */
int register_obj_allocator(int);
void *obj_allocate(int token);
void obj_free(void *obj_addr);
void dump_obj_alloc(obj_allocator_t *);

/**
 * Dynamic Memory Allocator - it's based on Buddy System and Object Allocator
 */
void __init_kmalloc();
void *kmalloc(int size);
void kfree(void *addr);

/**
 *  mm_init - Initialize system of memory management 
 */
void mm_init();

/* Functions after MMU enable*/
void *get_free_page();
void free_page(void *p);
void *alloacte_kernel_page();
void *alloacte_user_page(struct task_struct *task, unsigned long va);
void map_table_entry(unsigned long *pte, unsigned long va, unsigned long page);
unsigned long map_table (unsigned long *table, unsigned long shift, unsigned long va, int *new_table);
void map_page(struct task_struct *task, unsigned long va, unsigned long page);

int copy_virt_memory(struct task_struct *dst);

/**
 * page fault exception (or, which is the same, data access exception)
 * @param[in] addr  The memory address which we tried to access. This address is taken from far_el1 register (Fault address register)
 * @param[in] esr   The content of the esr_el1 (Exception syndrome register)
 * @return          0 if success, otherwise return non zero value
 * 
 * 
 * esr register - 
 * Bits [32:26] of this register are called "Exception Class".
 * We check those bits in the el0_sync handler to determine whether it is a syscall, or a data abort exception or potentially something else.
 * Exception class determines the meaning of bits [24:0] - those bits are usually used to provide additional information about the exception. 
 * The meaning of [24:0] bits in case of the data abort exception is described on the page 2460 of the AArch64-Reference-Manual
 */
int do_mem_abort(unsigned long addr, unsigned long esr);

/* mm.S */
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
void memzero(unsigned long src, unsigned long n);

#endif /* __ASSEMBLER__ */

#endif /* _MM_H */


