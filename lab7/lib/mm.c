#include <stdint.h>
#include <stddef.h>
#include <uart.h>
#include <list.h>
#include <string.h>
#include <interrupt.h>

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
#define CACHE_ORDER2SIZE(ord) (CACHE_MIN_SIZE * (1 << (ord)))

/*
   The implementation philosophy is always take down any node from linked list
   instantly after the free list in the node being empty, so we don't have to
   spend time traversing the list to find out which node's free list contains free chunk.
*/

struct freed {
    struct freed *next;
};

typedef struct freed freed_cache;
typedef struct freed freed_cache_meta;

struct cache_meta {
    freed_cache *free_cache;
    void *cache_page;
    struct list_head list;
    unsigned char order;
    int refcnt;
};

struct cache_meta_page {
    freed_cache_meta *freelist;
    struct list_head list;
    int refcnt;
};

struct frame {
    unsigned char flags;
    unsigned char order;
    struct cache_meta *cache;
};

static struct frame frame_array[FRAME_ARRAY_SIZE];
static struct list_head frame_bins[FRAME_BINS];
static struct list_head cache_meta_bins[CACHE_BINS];
struct list_head cache_meta_page_list = LIST_HEAD_INIT(cache_meta_page_list);

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
        frame_array[((uintptr_t)s - FRAME_BASE) / PAGE_SIZE].order = i - 1;

        #ifdef DEBUG
            print_uart("insert frame: ");
            write_hex_uart((unsigned long)s);
            write_uart("\r\n", 2);
        #endif
    }
    int idx = addr2idx(ptr);
    frame_array[idx].order = target_order;
    frame_array[idx].flags = FRAME_INUSE;
    return ptr;
}


/* Assuming total frame size can be divide by biggest bin size */
void init_buddy() {
    struct list_head *victim;

    for (int i = 0; i < FRAME_BINS; i++) {
        list_init(&frame_bins[i]);
    }

    for (uint64_t i = 0; i < FRAME_ARRAY_SIZE; i += 1 << MAX_ORDER) {
        frame_array[i].order = MAX_ORDER;

        victim = (struct list_head *)(FRAME_BASE + i * PAGE_SIZE);
        insert_tail(&frame_bins[MAX_ORDER], victim);

        #ifdef DEBUG
            print_uart("insert frame: ");
            write_hex_uart((unsigned long)victim);
            write_uart("\r\n", 2);
        #endif
    }
}

void init_cache() {
    for (int i = 0; i < CACHE_BINS; i++) {
        list_init(&cache_meta_bins[i]);
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
    if (!IS_INUSE(frame_array[page_idx])) {
        puts_uart("[Warn] Kernel: double free");
        return;
    }

    unsigned order = frame_array[page_idx].order;
    int buddy_page_idx = page_idx ^ (1 << order);
    frame_array[page_idx].flags &= ~FRAME_INUSE;

    /* merge frames */
    while (order < MAX_ORDER &&
           !IS_INUSE(frame_array[buddy_page_idx]) &&
           order == frame_array[buddy_page_idx].order)
    {
        void *buddy_victim = (void *)(FRAME_BASE + buddy_page_idx * PAGE_SIZE);
        unlink((struct list_head *)buddy_victim);

        #ifdef DEBUG
            print_uart("merge buddy frame: ");
            write_hex_uart((unsigned long)buddy_victim);
            write_uart("\r\n", 2);
        #endif

        order += 1;

        /* choose lowest frame as next merge victim */
        victim = page_idx < buddy_page_idx ? victim : buddy_victim;
        page_idx = page_idx < buddy_page_idx ? page_idx : buddy_page_idx;
        buddy_page_idx = page_idx ^ (1 << order);
    }

    insert_head(&frame_bins[order], victim);
    frame_array[page_idx].order = order;

    #ifdef DEBUG
        print_uart("attach frame: ");
        write_hex_uart((unsigned long)victim);
        write_uart("\r\n", 2);
    #endif
}

static void *get_cache(unsigned int size) {
    int order = size_to_cache_order(size);
    void *ptr = NULL;

    if (!list_empty(&cache_meta_bins[order])) {
        struct cache_meta *meta = list_first_entry(&cache_meta_bins[order], struct cache_meta, list);
        ptr = meta->free_cache;
        meta->free_cache = meta->free_cache->next;
        meta->refcnt++;

        /* unlink meta if there's no free cache */
        if (!meta->free_cache) {
            unlink(&meta->list);
        }

        #ifdef DEBUG
            print_uart("detach cache: ");
            write_hex_uart((unsigned long)ptr);
            write_uart("\r\n", 2);
        #endif
    }

    return ptr;
}

/* size should aligned to next exp size */
static struct cache_meta *alloc_cache_meta() {
    /* if there's no free meta page, allocate one */
    if (list_empty(&cache_meta_page_list)) {
        void *page = alloc_pages(1);
        struct cache_meta_page *meta_page = page;
        int count = (PAGE_SIZE - sizeof(struct cache_meta_page)) / sizeof(struct cache_meta);
        uintptr_t mem = (uintptr_t)page + sizeof(struct cache_meta_page);
        meta_page->refcnt = 0;
        meta_page->freelist = NULL;

        /* add all cache meta chunks into free list */
        for (int i = 0; i < count; i++) {
            freed_cache_meta *ptr = (freed_cache_meta *)(mem + i * sizeof(struct cache_meta));
            ptr->next = meta_page->freelist;
            meta_page->freelist = ptr;
        }

        list_init(&meta_page->list);
        insert_head(&cache_meta_page_list, &meta_page->list);
    }

    /* ensure cache_meta_page_list is not empty before here */

    struct cache_meta_page *page = list_first_entry(&cache_meta_page_list, struct cache_meta_page, list);
    struct cache_meta *p = (struct cache_meta *)page->freelist;
    page->freelist = page->freelist->next;
    page->refcnt++;

    /* take meta page out if all cache_meta on the page is inuse */
    if (!page->freelist) {
        unlink(&page->list);
    }

    return p;
}

static void free_cache_meta(struct cache_meta *p) {
    /* get corresponding cache_meta_page */
    struct cache_meta_page *meta_page = (struct cache_meta_page *)((uintptr_t)p & ~(PAGE_SIZE-1));

    /* if meta page is not on cache_meta_page_list (all inuse), add it here */
    if (!meta_page->freelist) {
        insert_tail(&cache_meta_page_list, &meta_page->list);
    }

    /* take cache meta out from cache_bins */
    unlink(&p->list);

    freed_cache_meta *fp = (freed_cache_meta *)p;

    fp->next = meta_page->freelist;
    meta_page->freelist = fp;
    meta_page->refcnt--;

    /* if all cache_meta on meta page got freed, free the meta page */
    if (!meta_page->refcnt) {
        /* detach meta page from cache_meta_page_list */
        unlink(&meta_page->list);
        free_pages(meta_page);
    }
}

/* size should aligned to next exp size */
static void alloc_cache(void *mem, int size) {
    int count = PAGE_SIZE / size;
    int idx = addr2idx(mem);
    int order = size_to_cache_order(size);
    struct cache_meta *meta = alloc_cache_meta();

    meta->order = order;
    meta->cache_page = mem;
    meta->free_cache = NULL;
    frame_array[idx].flags |= FRAME_MEM_CACHE;
    frame_array[idx].cache = meta;

    for (int i = 0; i < count; i++) {
        freed_cache *ptr = (freed_cache *)((uintptr_t)mem + i * size);
        ptr->next = meta->free_cache;
        meta->free_cache = ptr;

        #ifdef DEBUG
            print_uart("insert cache: ");
            write_hex_uart((unsigned long)ptr);
            write_uart("\r\n", 2);
        #endif
    }

    insert_head(&cache_meta_bins[order], &meta->list);
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

    size_t flag = disable_irq_save();

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

    irq_restore(flag);

    return cache;
}

void *kcalloc(unsigned int size) {
    void *p = kmalloc(size);
    if (!p) {
        return NULL;
    }
    if (size < PAGE_SIZE) {
        size = align_up_exp(size);
    } else {
        size = align_up(size, PAGE_SIZE);
    }
    memset(p, 0, size);

    return p;
}

size_t get_alloc_size(void *ptr) {
    if (!ptr) {
        return 0;
    }

    int idx = addr2idx(ptr);
    if (idx >= FRAME_ARRAY_SIZE) {
        return 0;
    }

    if (IS_MEM_CACHE(frame_array[idx])) {
        return CACHE_ORDER2SIZE(frame_array[idx].cache->order);
    } else {
        return ORDER2SIZE(frame_array[idx].order);
    }
}

void kfree(void *ptr) {
    if (!ptr) {
        return;
    }

    int idx = addr2idx(ptr);
    if (idx >= FRAME_ARRAY_SIZE) {
        puts_uart("[Warn] Kernel: kfree wrong address");
        return;
    }

    size_t flag = disable_irq_save();

    if (IS_MEM_CACHE(frame_array[idx])) {
        struct cache_meta *meta = frame_array[idx].cache;

        /* if it got unlinked before (all inuse), add it here */
        if (!meta->free_cache) {
            unlink(&meta->list);
        }

        ((freed_cache *)ptr)->next = meta->free_cache;
        meta->free_cache = ptr;
        meta->refcnt--;

        #ifdef DEBUG
            print_uart("attach cache: ");
            write_hex_uart((unsigned long)ptr);
            write_uart("\r\n", 2);
        #endif

        /* free the unused cache page */
        if (!meta->refcnt) {
            free_pages(meta->cache_page);
            free_cache_meta(meta);
        }

    } else {
        free_pages(ptr);
    }

    irq_restore(flag);
}