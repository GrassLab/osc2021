#include "mm.h"
#include "uart.h"
#include "stdint.h"
#include "stddef.h"


#define FRAME_BASE 0x10000000
#define FRAME_SIZE 0x10000000
#define PAGE_SIZE 4096
#define FRAME_ARRAY_SIZE (FRAME_SIZE / PAGE_SIZE)
#define FRAME_BINS 12
#define MAX_ORDER (FRAME_BINS - 1)
#define FRAME_SIZE_MAX ORDER2SIZE(MAX_ORDER)
#define ORDER2SIZE(ord) (PAGE_SIZE * (1 << (ord)))


/* cache start from 32 bytes to 2048 bytes */
#define CACHE_BINS 7
#define CACHE_MIN_SIZE 32
#define CACHE_MAX_ORDER (CACHE_BINS - 1)
//list operation

struct List_head{
	struct List_head *prev, *next;
};

struct List{
	struct List *next;
};

void List_insert_prev(struct List_head* now,struct List_head* node){
	node->prev = now->prev;
	node->next = now;
	now->prev->next = node;
	now->prev = node;
}

void List_insert_next(struct List_head* now, struct List_head* node){
	node->prev = now;
	node->next = now->next;
	now->next->prev = node;
	now->next = node;
}

struct List_head* List_move_next(struct List_head* now){
	struct List_head* node;
    node = now->next;
    now->next = now->next->next;
    now->next->prev = now;

    return node;
}

struct List_head* List_move_prev(struct List_head* now){
	struct List_head* node;
    node = now->prev;
    now->prev = now->prev->prev;
    now->prev->next = now;

    return node;
}

void unlink(struct List_head *node) {
    struct List_head *next, *prev;
    next = node->next;
    prev = node->prev;
    
    next->prev = prev;
    prev->next = next;
}

///////////////////////////////


struct frame_flag {
    int flags;
    int order;
    unsigned short refcnt;
    unsigned char cache_order;
};

static struct frame_flag frame[FRAME_ARRAY_SIZE];
static struct List_head frame_bins[FRAME_BINS];
static struct List *cache_bins[CACHE_BINS];
static uint32_t init = 0;
static int NUM[FRAME_BINS];
void init_buddy() {
    struct List_head *node;

    for (int i = 0; i < FRAME_BINS; i++) {
        frame_bins[i].next = &frame_bins[i];
        frame_bins[i].prev = &frame_bins[i];
        NUM[i] = 0; 
    }
    NUM[MAX_ORDER] = 1;
    
    for (uint64_t i = 0; i < FRAME_ARRAY_SIZE; i += 1 << MAX_ORDER) {
        frame[i].order = MAX_ORDER;

        node = (struct List_head *)(FRAME_BASE + i * PAGE_SIZE);
        List_insert_prev(&frame_bins[MAX_ORDER], node);
    }
}


void *split_frames(int now_order, int target_order) {
    
    struct List_head *node = List_move_next(&frame_bins[now_order]);
    uart_printf("start split order %d to %d\n",now_order, target_order);

    NUM[now_order] -= 1; 
    for (int i = now_order; i > target_order; i--) {
        struct List_head *s = (struct List_head *)((char *)node + PAGE_SIZE * (1 << (i-1)));
        List_insert_next(&frame_bins[i-1], s);

        frame[((uint64_t)s - FRAME_BASE) / PAGE_SIZE].order = i - 1;
        NUM[i-1] ++;
    }
   
    for(int i = 0; i < FRAME_BINS; i++){
        
        uart_printf("now order %d num is %d\n",i, NUM[i]);
    }
    int idx = (((uint64_t)node) - FRAME_BASE) / PAGE_SIZE;
    frame[idx].order = target_order;
    frame[idx].flags = 1;
    return node;
}

static void *alloc_pages(unsigned count) {
    if (!init) {
        init = 1;
        init_buddy();
    }

    int order = count - 1;

    for (int i = order; i < FRAME_BINS; i++) {
        if (frame_bins[i].next != &frame_bins[i]) {
        	
            return split_frames(i, order);
        }
        uart_printf("order %d is null \n",i);
    }

  
    return NULL;
}

void free_pages(void *addr) {
    int page_idx = ((uint64_t)addr - FRAME_BASE) / PAGE_SIZE;
    if (frame[page_idx].flags == 0) {
        uart_printf("[Warn] Kernel: double free");
        return;
    }
    
    unsigned order = frame[page_idx].order;

    int buddy_page_idx = page_idx ^ (1 << order);
    frame[page_idx].flags = 0;
    NUM[order]++;
    /* merge frames */
    uart_printf("%d %d %d %d %d**\n",order, frame[buddy_page_idx].order, frame[buddy_page_idx].flags, buddy_page_idx, page_idx);
    while (order < MAX_ORDER &&
           frame[buddy_page_idx].flags == 0 &&
           order == frame[buddy_page_idx].order)
    {
        void *buddy_victim = (void *)(FRAME_BASE + buddy_page_idx * PAGE_SIZE);
        unlink((struct List_head *)buddy_victim);
        NUM[order] -= 2;
        NUM[order+1] +=1;
        order += 1;
        addr = page_idx < buddy_page_idx ? addr : buddy_victim;
        page_idx = page_idx < buddy_page_idx ? page_idx : buddy_page_idx;
        buddy_page_idx = page_idx ^ (1 << order);
    }
    uart_printf("start free & merge\n");
    for(int i = 0; i < FRAME_BINS; i++){
        
        uart_printf("now order %d num is %d\n",i, NUM[i]);
    }
    List_insert_next(&frame_bins[order], addr);
    frame[page_idx].order = order;
}



static void *get_cache(unsigned int size) {
    int order = size / CACHE_MIN_SIZE - 1;

    void *addr = cache_bins[order];
    
    if (addr) {
        cache_bins[order] = cache_bins[order]->next;

        int idx = (((uint64_t)addr) - FRAME_BASE)/ PAGE_SIZE;
        frame[idx].refcnt += 1;
    }

    return addr;
}

static void alloc_cache(void *mem, int size) {

    
    int idx = (((uint64_t)mem) - FRAME_BASE) / PAGE_SIZE;
    int order = size / CACHE_MIN_SIZE - 1;
    int csize = CACHE_MIN_SIZE * (1 << order);
    int count = PAGE_SIZE / csize;
    frame[idx].flags = 2;
    frame[idx].refcnt = 0;
    frame[idx].cache_order = order;
    struct List* ptr = cache_bins[order];
    for (int i = 0; i < count; i++) {
        struct List *addr = (struct List *)((uint64_t)mem + i * csize);
        if(i == 0){
        	cache_bins[order] = addr;
        	ptr = cache_bins[order];
        }
        else{
	        ptr->next= addr;
	        ptr = ptr->next;
	        uart_printf("ptr %x \n",ptr);
	        //addr->next = cache_bins[order];
	        //cache_bins[order] = addr;
	    }
    }

}



void *kmalloc(uint64_t size) {
    if (!size || size > FRAME_SIZE_MAX) {
        return NULL;
    }

    if (size < CACHE_MIN_SIZE) {
        size = CACHE_MIN_SIZE;
    }
    uart_printf("size is %d\n",size);
    void *cache;
    if (size < PAGE_SIZE) {  
    uart_printf("size is %d\n",size); 
        cache = get_cache(size);

        if (!cache) {
        	uart_printf("size is %d\n",size);
            void *mem = alloc_pages(1);

            uart_printf("address %x\n", mem);
            alloc_cache(mem, size);
            cache = get_cache(size);
        }
    } else {
        unsigned pages = size / PAGE_SIZE;
        cache = alloc_pages(pages);
    }

    return cache;
}

void kfree(void *ptr) {
    int idx = (((uint64_t)ptr) - FRAME_BASE) / PAGE_SIZE;
    if (idx >= FRAME_ARRAY_SIZE) {
        uart_printf("[Warn] Kernel: kfree wrong address");
        return;
    }

    if (frame[idx].flags == 2) {
        int order = frame[idx].cache_order;
        ((struct List *)ptr)->next = cache_bins[order];
        cache_bins[order] = ptr;
        frame[idx].refcnt -= 1;

        /* find when to release unreferenced cache */

    } else {
        free_pages(ptr);
    }
}