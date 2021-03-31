#include "mm.h"
#include "uart.h"
#include "printf.h"


page_t bookkeep[PAGE_FRMAME_NUM];
free_area_t free_area[MAX_ORDER + 1];

obj_allocator_t obj_alloc_pool[MAX_OBJ_ALLOCTOR_NUM];

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
    printf("\n[buddy_block_alloc]Before allocate buddy memory:");
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
        printf("\n[buddy_block_alloc]After allocate buddy memory:");
        dump_buddy();
        #endif //__DEBUG
        
        printf("[buddy_block_alloc] Result - Allocated block{ pfn(%d), order(%d), phy_addr_16(0x%x) }\n",
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
    printf("\n[buddy_block_free]After free memory:");
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

void __init_obj_alloc(obj_allocator_t *obj_allocator_p, int objsize)
{
    INIT_LIST_HEAD(&obj_allocator_p->full);   
    INIT_LIST_HEAD(&obj_allocator_p->partial); 
    INIT_LIST_HEAD(&obj_allocator_p->empty);   
    obj_allocator_p->curr_page = NULL;
    
    obj_allocator_p->objsize = objsize;
    obj_allocator_p->obj_per_page = PAGE_SIZE / objsize;
    obj_allocator_p->obj_used = 0;
    obj_allocator_p->page_used = 0;
    
}

void __init_obj_page(page_t *page_p) 
{
    page_p->obj_used = 0;
    page_p->free = NULL;
}

int register_obj_allocator(int objsize) 
{
    if (objsize < MIN_ALLOCATAED_OBJ_SIZE) {
        objsize = MIN_ALLOCATAED_OBJ_SIZE;
        printf("[register_obj_allocator] Min object size is 8, automatically set it to 8 ");
    }

    if (objsize > MAX_ALLOCATAED_OBJ_SIZE) {
        objsize = MAX_ALLOCATAED_OBJ_SIZE;
        printf("[register_obj_allocator] Max object size is 2048, automatically set it to 2048 ");
    }

    for (int token = 0;token < MAX_OBJ_ALLOCTOR_NUM;token++) {
        if (obj_alloc_pool[token].objsize != 0) 
            continue;

        __init_obj_alloc(&obj_alloc_pool[token], objsize);

        #ifdef __DEBUG
        printf("[register_obj_allocator] Successfully Register object allocator! {objsize(%d), token(%d)}\n"
                ,objsize, token);
        #endif //__DEBUG 

        return token;
    }

    printf("[register_obj_allocator] Allocator pool has been fully registered.");
    return -1;
}

void *obj_allocate(int token) {
    if (token < 0 || token >= MAX_OBJ_ALLOCTOR_NUM) {
        printf("[obj allocator] Invalid token\n");
        return 0;
    }    
    
    obj_allocator_t *obj_allocator_p = &obj_alloc_pool[token];
    void *allocated_addr = NULL; // address of allocated object 

    #ifdef __DEBUG
    printf("[obj_allocate] Requested token: %d, size: %d\n",token, obj_allocator_p->objsize);
    printf("[obj_allocate] Before allocation:");
    dump_obj_alloc(obj_allocator_p);
    #endif //__DEBUG
    
    if (obj_allocator_p->curr_page == NULL) {
        page_t *page_p;
        if (!list_empty(&obj_allocator_p->partial)) { 
            // Use partial allocated page
            page_p = (page_t *) obj_allocator_p->partial.next;
            list_del(&page_p->list);
        } else if (!list_empty(&obj_allocator_p->empty)) {
            // Use empty(no alloacted object) page
            page_p = (page_t *) obj_allocator_p->empty.next;
            list_del(&page_p->list);
        } else {
            // Demand a new page from buddy memory allocator
            page_p = buddy_block_alloc(0);
            __init_obj_page(page_p);
            page_p->obj_alloc = obj_allocator_p;

            obj_allocator_p->page_used += 1;
        }
        obj_allocator_p->curr_page = page_p;
    }

    struct list_head *obj_freelist = obj_allocator_p->curr_page->free;
    if (obj_freelist != NULL) {
        // Allocate memory by free list in current page
        allocated_addr = obj_freelist;
        obj_freelist = obj_freelist->next; // Point to next address of free object;
    }
    else {
        // Allocate memory to requested object
        allocated_addr = (void *) obj_allocator_p->curr_page->phy_addr + 
                         obj_allocator_p->curr_page->obj_used * obj_allocator_p->objsize;
    }

    obj_allocator_p->obj_used += 1;
    obj_allocator_p->curr_page->obj_used += 1;

    // Check if page full
    if (obj_allocator_p->obj_per_page == obj_allocator_p->curr_page->obj_used) {
        list_add_tail(&obj_allocator_p->curr_page->list, &obj_allocator_p->full);
        obj_allocator_p->curr_page = NULL;
    }

    #ifdef __DEBUG
    printf("[obj_allocate] After allocation:");
    dump_obj_alloc(obj_allocator_p);
    printf("[obj_allocate] **done**\n");
    #endif //__DEBUG

    printf("\n\n[obj_allocate] Allocated address: {phy_addr_16(%x)}\n", allocated_addr);
    return allocated_addr;
    

}

void obj_free(void *obj_addr) {
    // Find out corressponding page frame number and object allocator it belongs to.
    int obj_pfn = PHY_ADDR_TO_PFN(obj_addr);
    page_t *page_p = &bookkeep[obj_pfn];
    obj_allocator_t *obj_allocator_p = page_p->obj_alloc;
    
    #ifdef __DEBUG
    printf("\n[obj_free] Free object procedure!\n");
    printf("[obj_free] Page info: 0x%x {pfn=(%d), obj_used(%d))\n", obj_addr, obj_pfn, page_p->obj_used);
    printf("[obj_free] object free list point to {0x%x}\n", page_p->free);
    printf("[obj_free] Before free:");
    dump_obj_alloc(obj_allocator_p);
    #endif // __DEBUG

    // Make page's object freelist point to address of new frist free object
    struct list_head *temp = page_p->free;
    page_p->free = (struct list_head *) obj_addr;
    page_p->free->next = temp;

    obj_allocator_p->obj_used -= 1;
    page_p->obj_used -= 1;

    // From full to partial 
    if (obj_allocator_p->obj_per_page-1 == page_p->obj_used) {
        list_del(&page_p->list); // pop out from full list
        list_add_tail(&page_p->list, &obj_allocator_p->partial); // add to partial list 
    }

    // From partial to empty
    // and make sure this page not currently used by object allocator
    if (page_p->obj_used == 0 && obj_allocator_p->curr_page != page_p) {
        list_del(&page_p->list); // pop out from partial list
        list_add_tail(&page_p->list, &obj_allocator_p->empty);
    }

    #ifdef __DEBUG
    printf("[obj_free] object free list point to {0x%x}\n", page_p->free);
    printf("[obj_free] After free:");
    dump_obj_alloc(obj_allocator_p);
    #endif // __DEBUG
}

#ifdef __DEBUG
void dump_obj_alloc(obj_allocator_t *obj_allocator_p)
{
    printf("\n---------Object Allocator Debug---------\n");
    printf("objsize = %d\n", obj_allocator_p->objsize);
    printf("obj_per_page = %d\n", obj_allocator_p->obj_per_page);
    printf("obj_used = %d\n", obj_allocator_p->obj_used);
    printf("page_used = %d\n", obj_allocator_p->page_used);

    
    printf("\nobject_allocator->curr_page current page info:\n");
    if (obj_allocator_p->curr_page != NULL) {
        printf("obj_allocator_p->curr_page = {0x%x}\n", obj_allocator_p->curr_page->phy_addr);
        printf("obj free list point to {0x%x}\n", obj_allocator_p->curr_page->free);
        printf("obj_used = %d\n", obj_allocator_p->curr_page->obj_used);
        printf("pfn = %d\n", obj_allocator_p->curr_page->pfn);
        
    }
    else {
        printf("object_allocator->curr_page is NULL currently\n");
    }
    printf("\n");

    struct list_head *pos;
    printf("object_allocator->full list:\n");
    list_for_each(pos, (struct list_head *) &obj_allocator_p->full) {
        printf("--> {pfn(%d)}", ((struct page*) pos)->pfn);
    }
    printf("\n");

    printf("object_allocator->partial list:\n");
    list_for_each(pos, (struct list_head *) &obj_allocator_p->partial) {
        printf("--> {pfn(%d)}", ((struct page*) pos)->pfn);
    }
    printf("\n");

    printf("object_allocator->empty list:\n");
    list_for_each(pos, (struct list_head *) &obj_allocator_p->empty) {
        printf("--> {pfn(%d)}", ((struct page*) pos)->pfn);
    }

    printf("\n---------End Object Allocator Debug---------\n\n");

}
#endif


void mm_init()
{
    page_init();
    free_area_init();
    
    
    /**
     *  Test Buddy memory Allocator
     */
    // int allocate_test1[] = {5, 0, 6, 3, 0};
    // int test1_size = sizeof(allocate_test1) / sizeof(int);
    // page_t *(one_pages[test1_size]);
    // for (int i = 0;i < test1_size;i++) {
    //     page_t *one_page = buddy_block_alloc(allocate_test1[i]); // Allocate one page frame
    //     //printf("\n Allocated Block{ pfn(%d), order(%d), phy_addr_16(0x%x) }: %u\n", one_page->pfn, one_page->order, one_page->phy_addr);
    //     one_pages[i] = one_page;
    // }
    // buddy_block_free(one_pages[2]);
    // buddy_block_free(one_pages[1]);
    // buddy_block_free(one_pages[4]);
    // buddy_block_free(one_pages[3]);
    // buddy_block_free(one_pages[0]);

    /**
     *  Test object allcator
     */
    int token = register_obj_allocator(2000);
    void *addr1 = obj_allocate(token); // 0
    void *addr2  = obj_allocate(token);
    
    void *addr3 = obj_allocate(token); // 1
    void *addr4 = obj_allocate(token);

    void *addr5 = obj_allocate(token); // 2
    void *addr6 = obj_allocate(token);

    void *addr7 = obj_allocate(token); // 3
    void *addr11 = obj_allocate(token);

    void *addr12 = obj_allocate(token); // 4
    void *addr13 = obj_allocate(token);
    obj_free(addr1);
    obj_free(addr2);
    void *addr14 = obj_allocate(token); // 0
    void *addr15 = obj_allocate(token);

    void *addr16 = obj_allocate(token); // 5
    
    obj_free(addr11);
    obj_free(addr5);
    obj_free(addr15);
    obj_free(addr3);

    void *addr17 = obj_allocate(token); // 5
    void *addr18 = obj_allocate(token); // 3
}