#include "mm.h"
#include "uart.h"
#include "printf.h"


page_t bookkeep[PAGE_FRMAME_NUM];
free_area_t free_area[MAX_ORDER + 1];

void page_init() 
{
    for (int i = 0;i < PAGE_FRMAME_NUM;i++) {
        bookkeep[i].pfn = i;
        bookkeep[i].used = Free;
        bookkeep[i].phy_addr = LOW_MEMORY + i*PAGE_SIZE;
        bookkeep[i].order = -1; 
    }
}

void free_area_init()
{
    for (int i = 0;i < MAX_ORDER + 1;i++) {
        INIT_LIST_HEAD(&free_area[i].freelist);
    }

    for (int i = 0;i < PAGE_FRMAME_NUM;i += MAX_ORDER_SIZE) {
        push_block_to_free_area(&bookkeep[i], &free_area[MAX_ORDER], MAX_ORDER);
    }
}

void push_block_to_free_area(page_t *pushed_block, free_area_t *fa, int order) 
{
    list_add_tail(&pushed_block->list, &fa->freelist);
    pushed_block->order = order;
    pushed_block->used = Free;
    fa->nr_free += 1;
}

void pop_block_from_free_area(page_t *poped_block, free_area_t *fa) {
    list_del(&poped_block->list);
    poped_block->used = Taken;
    fa->nr_free--;
}

struct page *buddy_block_alloc(int order) 
{
    #ifdef __DEBUG
    uart_puts("\n[buddy_block_alloc]Before allocated memory:");
    dump_buddy();
    #endif //__DEBUG

    if ( (order<0) | (order>MAX_ORDER) ) {
        printf("[buddy_block_alloc] %d is invalid order!\n", order);
        return 0;
    }

    printf("[buddy_block_alloc] Requested Order: %d, Size: %d\n", order, 1 << order);

    for (int i = order;i <= MAX_ORDER;i++) {
        if (list_empty(&free_area[i].freelist)) continue;
        
        // Found target block, get and remove one block from target freelist
        struct page *target_block = (struct page *) free_area[i].freelist.next;
        pop_block_from_free_area(target_block, &free_area[i]);
        target_block->order = order;

        // Cut off bottom half of the block and put it back to corresponding freelist
        // until the size equals the requested order. 
        for (int current_order = i;current_order > order;current_order--) {
            int downward_order = current_order - 1; 

            int buddy_pfn = FIND_BUDDY_PFN(target_block->pfn, downward_order);
            struct page *bottom_half_block = &bookkeep[buddy_pfn];
            push_block_to_free_area(bottom_half_block, &free_area[downward_order], downward_order);
            printf("Push back -> Redundant block(bottom half)  { pfn(%d), order(%d) }\n", 
                    bottom_half_block->pfn, bottom_half_block->order);
        }

        #ifdef __DEBUG   
        printf("\n[buddy_block_alloc]After allocated memory:");
        dump_buddy();
        #endif //__DEBUG
        
        printf("[buddy_block_alloc] Result - Allocated block{ pfn(%d), order(%d), phy_addr_16(%x)}\n",
                target_block->pfn, target_block->order, target_block->phy_addr);
        printf("[buddy_block_alloc] **done**\n\n");
        return target_block;
        
    }

    
    printf("[buddy_block_alloc] No free memory space!\n");
    return 0;
}

void buddy_block_free(struct page* block) 
{
    printf("\n[buddy_block_free] **Start free block{ pfn(%d), order(%d) }**\n", 
           block->pfn, block->order);

    #ifdef __DEBUG
    printf("\n[buddy_block_free]Before free memory:");
    dump_buddy();
    #endif //__DEBUG

    // Coalesce free buddy
    int buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
    page_t *buddy_block = &bookkeep[buddy_pfn];
    while (block->order < MAX_ORDER && block->order == buddy_block->order && 
           buddy_block->used == Free) {
        printf("Buddy{ pfn(%d), order(%d) }\n", buddy_block->pfn, buddy_block->order);

        // Pop buddy block from frealist
        pop_block_from_free_area(buddy_block, &free_area[buddy_block->order]);

        // Find left block as primary block
        int lbuddy_pfn = FIND_LBUDDY_PFN(block->pfn, block->order);
        block = &bookkeep[lbuddy_pfn];
        
        // Add 1 to order in primary block. It's means that it merge two block. 
        block->order += 1;
        
        // prepare next merge iteration
        buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
        buddy_block = &bookkeep[buddy_pfn];
    } 
    // Push merged block to freelist
    push_block_to_free_area(block, &free_area[block->order], block->order);

    #ifdef __DEBUG
    uart_puts("\n[buddy_block_free]After free memory:");
    dump_buddy();
    #endif //__DEBUG

    printf("[buddy_block_free] **done**\n\n");
}


#ifdef __DEBUG
void dump_buddy()
{
    printf("\n---------Buddy Debug---------\n");
    printf("***Freelist(free_area) Debug***");
    for (int i = 0;i < MAX_ORDER+1 ;i++) {
        printf("\nOrder-%d\n", i);

        struct list_head *pos;
        list_for_each(pos, (struct list_head *) &free_area[i].freelist) {
            printf(" -> {pfn(%d)} {phy_addr_16(0x%x)}", ((struct page *)pos)->pfn, ((struct page *)pos)->phy_addr);
        }
        
            
    }
    printf("\n---------End Buddy Debug---------\n\n");

}
#endif


void mm_init()
{
    page_init();
    free_area_init();

    /**
     *  Test Buddy memory Allocator
     */
    int allocate_test1[] = {5, 0, 6, 3, 0};
    int test1_size = sizeof(allocate_test1) / sizeof(int);
    page_t *(one_pages[test1_size]);
    for (int i = 0;i < test1_size;i++) {
        page_t *one_page = buddy_block_alloc(allocate_test1[i]); // Allocate one page frame
        //printf("\n Allocated Block{ pfn(%d), order(%d), phy_addr_16(0x%x) }: %u\n", one_page->pfn, one_page->order, one_page->phy_addr);
        one_pages[i] = one_page;
    }
    buddy_block_free(one_pages[2]);
    buddy_block_free(one_pages[1]);
    buddy_block_free(one_pages[4]);
    buddy_block_free(one_pages[3]);
    buddy_block_free(one_pages[0]);
    
}