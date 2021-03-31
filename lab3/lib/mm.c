#include <stdint.h>
#include <stddef.h>
#include "include/uart.h"
#include "include/list.h"

#define FRAME_BASE ((uintptr_t) 0x10000000)
#define FRAME_END ((uintptr_t) 0x20000000)

#define FRAME_ARRAY_SIZE ((FRAME_END - FRAME_BASE) / PAGE_SIZE)
#define PAGE_SIZE 0x1000
#define FRAME_BINS 12
#define MAX_ORDER (FRAME_BINS - 1)
#define FRAME_SIZE_MAX ORDER2SIZE(MAX_ORDER)
#define ORDER2SIZE(ord) (PAGE_SIZE * (1 << (ord)))

/* we dont need free tag */
#define FRAME_FREE 0x8
#define FRAME_INUSE 0x4
#define FRAME_MEM_CACHE 0x2

#define IS_INUSE(flag) ((flag).flags & FRAME_INUSE)
#define IS_MEM_CACHE(flag) ((flag).flags & FRAME_MEM_CACHE)

/* cache start from 32 bytes to 2048 bytes */
#define CACHE_BINS 7
#define CACHE_MIN_SIZE 32
#define CACHE_MAX_ORDER (CACHE_BINS - 1)

struct frame_flag {
    unsigned char flags;
    unsigned char order;
    unsigned short refcnt;
    unsigned char cache_order;
};

static struct frame_flag frame[FRAME_ARRAY_SIZE];
static struct list_head frame_bins[FRAME_BINS];
static struct list *cache_bins[CACHE_BINS];
static uint32_t init = 0;

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static int addr2idx(void *addr) {
    return (((uintptr_t)addr & -PAGE_SIZE) - FRAME_BASE) / PAGE_SIZE;
}

static unsigned align_up_exp(unsigned n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

static void *split_frames(int order, int target_order) {
    /* take the ready to split frame out */
    struct list_head *ptr = remove_head(&frame_bins[order]);

#ifdef DEBUG
    print_uart("split frame: ");
    write_hex_uart((unsigned long)ptr);
    write_uart("\r\n", 2);
#endif
    /* puts splitted frame into bin list */
    for (int i = order; i > target_order; i--) {
        struct list_head *s = (struct list_head *)((char *)ptr + ORDER2SIZE(i-1));
        insert_head(&frame_bins[i-1], s);
        frame[((uintptr_t)s - FRAME_BASE) / PAGE_SIZE].order = i - 1;
    }
    int idx = addr2idx(ptr);
    frame[idx].order = target_order;
    frame[idx].flags = FRAME_INUSE;
    return ptr;
}


/* Assuming total frame size can be divide by biggest bin size */
static void init_buddy() {
    struct list_head *victim;

    for (int i = 0; i < FRAME_BINS; i++) {
        list_init(&frame_bins[i]);
    }
    
    for (uint64_t i = 0; i < FRAME_ARRAY_SIZE; i += 1 << MAX_ORDER) {
        frame[i].order = MAX_ORDER;

        victim = (struct list_head *)(FRAME_BASE + i * PAGE_SIZE);
        insert_tail(&frame_bins[MAX_ORDER], victim);
    }
}

static int pages_to_frame_order(unsigned count) {
    count = align_up_exp(count);
    return __builtin_ctz(count);
}
static int size_to_cache_order(unsigned size) {
    size = align_up_exp(size);
    size /= CACHE_MIN_SIZE;
    return __builtin_ctz(size);
}

static void *alloc_pages(unsigned count) {
    if (!init) {
        init = 1;
        init_buddy();
    }

    int target_order = pages_to_frame_order(count);

    for (int i = target_order; i < FRAME_BINS; i++) {
        if (frame_bins[i].next != &frame_bins[i]) {
            return split_frames(i, target_order);
        }
    }

    /* TODO: OOM handling */
    return NULL;
}

static void free_pages(void *victim) {
    int page_idx = ((uintptr_t)victim - FRAME_BASE) / PAGE_SIZE;
    if (!IS_INUSE(frame[page_idx])) {
        puts_uart("[Warn] Kernel: double free");
        return;
    }
    
    unsigned order = frame[page_idx].order;
    int buddy_page_idx = page_idx ^ (1 << order);
    frame[page_idx].flags &= ~FRAME_INUSE;

    /* merge frames */
    while (order <= MAX_ORDER &&
           !IS_INUSE(frame[buddy_page_idx]) &&
           order == frame[buddy_page_idx].order)
    {
        void *buddy_victim = (void *)(FRAME_BASE + buddy_page_idx * PAGE_SIZE);
        unlink((struct list_head *)buddy_victim);

        #ifdef DEBUG
            print_uart("merge buddy frame: ");
            write_hex_uart((unsigned long)buddy_victim);
            write_uart("\r\n", 2);
        #endif

        order += 1;
        victim = page_idx < buddy_page_idx ? victim : buddy_victim;
        page_idx = page_idx < buddy_page_idx ? page_idx : buddy_page_idx;
        buddy_page_idx = page_idx ^ (1 << order);
    }

    insert_head(&frame_bins[order], victim);
    frame[page_idx].order = order;
}

static void *get_cache(unsigned int size) {
    int order = size_to_cache_order(size);

    void *ptr = cache_bins[order];
    if (ptr) {
        cache_bins[order] = cache_bins[order]->next;
        int idx = addr2idx(ptr);
        frame[idx].refcnt += 1;
    }

    return ptr;
}

static void alloc_cache(void *mem, int size) {
    int count = PAGE_SIZE / size;
    int idx = addr2idx(mem);
    int order = size_to_cache_order(size);
    frame[idx].flags |= FRAME_MEM_CACHE;
    frame[idx].refcnt = 0;
    frame[idx].cache_order = order;

    for (int i = 0; i < count; i++) {
        struct list *ptr = (struct list *)((uintptr_t)mem + i * size);
        ptr->next = cache_bins[order];
        cache_bins[order] = ptr;
    }
}

/* request <  PAGE_SIZE: page memory pool
 * request >= PAGE_SIZE: only use alloc_page
 */
void *kmalloc(unsigned int size) {
    if (align_up(size, PAGE_SIZE) > FRAME_SIZE_MAX) {
        return NULL;
    }

    if (size < CACHE_MIN_SIZE) {
        size = CACHE_MIN_SIZE;
    }

    void *cache;
    if (align_up_exp(size) < PAGE_SIZE) {   
        size = align_up_exp(size);
        cache = get_cache(size);

        if (!cache) {
            void *mem = alloc_pages(1);
            alloc_cache(mem, size);
            cache = get_cache(size);
        }
    } else {
        unsigned pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
        cache = alloc_pages(pages);
    }

    return cache;
}

void kfree(void *ptr) {
    int idx = addr2idx(ptr);
    if (idx >= FRAME_ARRAY_SIZE) {
        puts_uart("[Warn] Kernel: kfree wrong address");
        return;
    }

    if (IS_MEM_CACHE(frame[idx])) {
        int order = frame[idx].cache_order;
        ((struct list *)ptr)->next = cache_bins[order];
        cache_bins[order] = ptr;
        frame[idx].refcnt -= 1;

        /* find when to release unreferenced cache */

    } else {
        free_pages(ptr);
    }
}