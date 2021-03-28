#ifndef _MM_H
#define _MM_H
#include "list.h"
#include "types.h"

#define PAGE_SHIFT                 12
#define PAGE_SIZE                  (1 << PAGE_SHIFT) // 4KB per page frame

#define PAGE_FRMAME_NUM            4096

#define MAX_ORDER                  9 
#define MAX_ORDER_SIZE             (1 << MAX_ORDER)

#define LOW_MEMORY                 0x90000

#define FIND_BUDDY_PFN(pfn, order) ((pfn) ^ (1<<(order)))
#define FIND_LBUDDY_PFN(pfn, order)((pfn) & (~(1<<(order))))
enum booking_status {
    Free, 
    Taken
};

typedef struct free_area_struct {
    int nr_free;
    struct list_head freelist;    
} free_area_t;

typedef struct page {
    struct list_head list;
    
    int order;
    int pfn; // page frame number
    int used;
    uint64_t phy_addr;
} page_t;


/**
 *  Initalization for Buddy system
 */
void page_init();
void free_area_init();
void dump_buddy();

/**
 *  buddy_block_alloc - Allocate page frames in specify page order
 *  @order: The Requested size of 2^order of page frames. 
 */
struct page *buddy_block_alloc(int order);


/**
 *  buddy_block_free - Free page frames 
 *  @order: Start address of page frames
 */
void buddy_block_free(struct page* block);

void push_block_to_free_area(page_t *, free_area_t *, int order);
void pop_block_from_free_area(page_t *, free_area_t *);


/**
 *  mm_init - Initialize system of memory management 
 */
void mm_init();

#endif /* _MM_H */


