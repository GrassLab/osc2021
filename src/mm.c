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
    // printf("\n[buddy_block_alloc]Before allocate buddy memory:");
    // dump_buddy();
    printf("[buddy_block_alloc] Requested Order: %d, Size: %d\n\n", order, 1 << order);
    #endif //__DEBUG

    if ( (order<0) | (order>MAX_ORDER) ) {
        printf("[buddy_block_alloc] %d is invalid order!\n", order);
        return 0;
    }

   

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

            #ifdef __DEBUG   
            printf("Push back -> Redundant block(bottom half)  { pfn(%d), order(%d) }\n", 
                    bottom_half_block->pfn, bottom_half_block->order);
            #endif //__DEBUG
        }

        #ifdef __DEBUG   
        printf("\n[buddy_block_alloc]After allocate buddy memory:");
        dump_buddy();
        
        printf("[buddy_block_alloc] Result - Allocated block{ pfn(%d), order(%d), phy_addr_16(0x%x) }\n",
                target_block->pfn, target_block->order, target_block->phy_addr);
        printf("[buddy_block_alloc] **Done**\n\n");
        #endif //__DEBUG

        return target_block;
        
    }

    
    printf("[buddy_block_alloc] No free memory space!\n");
    return 0;
}

void buddy_block_free(struct page* block) 
{
    #ifdef __DEBUG
     printf("\n[buddy_block_free] **Start free block{ pfn(%d), order(%d) }**\n", 
           block->pfn, block->order);
    // printf("\n[buddy_block_free]Before free memory:");
    // dump_buddy();
    #endif //__DEBUG

    // Coalesce free buddy
    int buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
    page_t *buddy_block = &bookkeep[buddy_pfn];
    while (block->order < MAX_ORDER && block->order == buddy_block->order && 
           buddy_block->used == Free) {
        #ifdef __DEBUG
        printf("Buddy{ pfn(%d), order(%d) }\n", buddy_block->pfn, buddy_block->order);
        #endif //__DEBUG
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
    printf("[buddy_block_free] **Done**\n\n");
    #endif //__DEBUG

    
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
    page_p->obj_alloc = NULL;
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
    // printf("[obj_allocate] Before allocation:");
    // dump_obj_alloc(obj_allocator_p);
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

    /* TODO: Explain how obj_freelist work*/
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
    printf("[obj_allocate] Allocated address: {phy_addr_16(%x)}\n", allocated_addr);
    printf("[obj_allocate] After allocation:");
    dump_obj_alloc(obj_allocator_p);
    printf("[obj_allocate] **Done**\n\n");
    #endif //__DEBUG

    
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
    // printf("[obj_free] Before free:");
    // dump_obj_alloc(obj_allocator_p);
    #endif // __DEBUG

    // Make page's object freelist point to address of new first free object.
    // And the contect of released object should record the orginal address 
    // of first free object that object freelist previously point to.
    // As the result, if we want to access second free object, just 
    // using free->next(first 8 bytes) to get expected address. 
    // So we can link and access all free object by this strategy without extra
    // moemory space.
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

        // Return empty page to free page pool(free_area)
        // otherwise, add it to empty list 
        if (obj_allocator_p->page_used >= 10) { // TODO: Return empty page only if memory becomes tight
            #ifdef __DEBUG
            printf("[obj_free] Free empty page bacause memory is tight");
            #endif // __DEBUG

            obj_allocator_p->page_used -= 1;
            page_p->free = NULL;
            page_p->obj_alloc = NULL;
            page_p->obj_used = 0;
            buddy_block_free(page_p);
        } else {
            // Add to empty list
            list_add_tail(&page_p->list, &obj_allocator_p->empty);
        }
    }

    #ifdef __DEBUG
    printf("[obj_free] After free:");
    dump_obj_alloc(obj_allocator_p);
    printf("[obj_free] **Done**\n\n");
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

void __init_kmalloc()
{
    for (int i = MIN_KMALLOC_ORDER;i <= MAX_KMALLOC_ODER;i++) {
        register_obj_allocator(1 << i);
    }
}

void *kmalloc(int size)
{
    #ifdef __DEBUG
    printf("[kmalloc] Requested Size: %d\n", size);
    #endif //__DEBUG

    void *allocated_addr;

    // Object allocator
    for (int i = MIN_KMALLOC_ORDER;i <= MAX_KMALLOC_ODER;i++) {
        if (size <= (1<<i)) {
            allocated_addr = obj_allocate(i - MIN_KMALLOC_ORDER);

            #ifdef __DEBUG
            printf("[kmlloc] Allocated address: 0x%x\n", allocated_addr);
            printf("[kmlloc] **Done**\n\n");
            #endif //__DEBUG

            return allocated_addr;
        }
    }
    // Buddy Memory allocator
    for (int i = 0;i <= MAX_ORDER;i++) {
        if (size <= 1<<(i + PAGE_SHIFT)) {
            allocated_addr = (void *) buddy_block_alloc(i)->phy_addr;

            #ifdef __DEBUG
            printf("[kmlloc] Allocated address: 0x%x\n", allocated_addr);
            printf("[kmlloc] **Done**\n\n");
            #endif //__DEBUG

            return allocated_addr;
        }
    }
    
    printf("[kmalloc] %d Bytes too large!\n", size);
    return NULL;
}

void kfree(void *addr) 
{
    #ifdef __DEBUG
    printf("[kfree] Free Memory Address: 0x%x\n", addr);
    #endif //__DEBUG

    int pfn = PHY_ADDR_TO_PFN(addr);
    page_t *page_p = &bookkeep[pfn];

    if (page_p->obj_alloc != NULL) {
        // Belongs to Object Allocator
        obj_free(addr);
    } else {
        // Belongs to Buddy Memory Allocator
        buddy_block_free(page_p);
    }

    #ifdef __DEBUG
    printf("[kfree] **Done**\n\n");
    #endif //__DEBUG
}

void mm_init()
{
    page_init();
    free_area_init();
    __init_kmalloc();
    
    /**
     *  Test Buddy memory Allocator
     */
    int allocate_test1[] = {1};
    int test1_size = sizeof(allocate_test1) / sizeof(int);
    page_t *(one_pages[test1_size]);
    for (int i = 0;i < test1_size;i++) {
        page_t *one_page = buddy_block_alloc(allocate_test1[i]); // Allocate one page frame
        //printf("\n Allocated Block{ pfn(%d), order(%d), phy_addr_16(0x%x) }: %u\n", one_page->pfn, one_page->order, one_page->phy_addr);
        one_pages[i] = one_page;
    }
    buddy_block_free(one_pages[0]);

    /**
     *  Test object allcator
     */
    // int token = register_obj_allocator(2000);
    // void *addr1 = obj_allocate(token); // page frame 0
    // void *addr2  = obj_allocate(token);
    
    // void *addr3 = obj_allocate(token); // 1
    // void *addr4 = obj_allocate(token);

    // void *addr5 = obj_allocate(token); // 2
    // void *addr6 = obj_allocate(token);

    // void *addr7 = obj_allocate(token); // 3
    // void *addr11 = obj_allocate(token);

    // void *addr12 = obj_allocate(token); // 4
    // void *addr13 = obj_allocate(token);
    // obj_free(addr1);
    // obj_free(addr2);
    // void *addr14 = obj_allocate(token); // 0
    // void *addr15 = obj_allocate(token);__init_kmalloc();
    // obj_free(addr11);
    // obj_free(addr5);
    // obj_free(addr15);
    // obj_free(addr3);

    // void *addr17 = obj_allocate(token); // 5
    // void *addr18 = obj_allocate(token); // 3
    
    
    /* Test Dynamic Memory Allocator */
    // __init_kmalloc();
    // // Test case 1
    // void *k_addr1 = kmalloc(16);
    // void *k_addr2 = kmalloc(48);
    // kfree(k_addr1);
    // void *k_addr3 = kmalloc(2048);
    // void *k_addr4 = kmalloc(2048);
    // void *k_addr5 = kmalloc(8787);
    // kfree(k_addr3);
    // kfree(k_addr4);
    // kfree(k_addr5);
    // Test case 2
    // void *address_1 = kmalloc(16);
    // void *address_2 = kmalloc(64);
    // kfree(address_1);
    // void *address_3 = kmalloc(1024);
    // kfree(address_2);
    // kfree(address_3);
    // void *address_4 = kmalloc(16);
    // void *address_9 = kmalloc(16384);
    // void *address_10 = kmalloc(16384);
    // kfree(address_4);
    // void *address_5 = kmalloc(32);
    // void *address_6 = kmalloc(32);
    // kfree(address_5);
    // kfree(address_6);
    // void *address_7 = kmalloc(512);
    // void *address_8 = kmalloc(512);
    // kfree(address_8);
    // kfree(address_7);
    // kfree(address_9);
    // kfree(address_10);
    // void *address_11 = kmalloc(8192);
    // void *address_12 = kmalloc(65536);
    // void *address_13 = kmalloc(128);
    // kfree(address_11);
    // void *address_14 = kmalloc(65536);
    // kfree(address_13);
    // kfree(address_12);
    // kfree(address_14);
    // void *address_15 = kmalloc(256);
    // kfree(address_15);
}