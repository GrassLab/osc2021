#include <stdint.h>
#include <stddef.h>
#include "include/uart.h"

#define FRAME_BASE ((uintptr_t) 0x10000000)
#define FRAME_END ((uintptr_t) 0x20000000)

#define FRAME_ARRAY_SIZE ((FRAME_END - FRAME_BASE) / PAGE_SIZE)
#define PAGE_SIZE 0x1000
#define FRAME_BINS 12
#define MAX_ORDER (FRAME_BINS - 1)
#define FRAME_SIZE_MAX ORDER2SIZE(MAX_ORDER)
#define ORDER2SIZE(ord) (PAGE_SIZE * (1 << (ord)))

/* what's the meaning of tagging ? */
#define FRAME_FREE 0x80000000
#define FRAME_INUSE 0x40000000
#define FRAME_FLAG_MASK 0x0fffffff

#define IS_INUSE(flag) ((flag) & FRAME_INUSE)
#define FRAME_ORDER(flag) ((flag) & FRAME_FLAG_MASK)

struct list_head {
    struct list_head *next, *prev;
};

static uint32_t frame[FRAME_ARRAY_SIZE];
static struct list_head bins[FRAME_BINS];
static uint32_t init = 0;

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}


static void insert_head(struct list_head *head, struct list_head *v) {
#ifdef DEBUG
    print_uart("insert frame at ");
    write_hex_uart((unsigned long)v);
    write_uart("\r\n", 2);
#endif
    v->next = head->next;
    v->prev = head;
    head->next->prev = v;
    head->next = v;
}

static void insert_tail(struct list_head *head, struct list_head *v) {
#ifdef DEBUG
    print_uart("insert frame: ");
    write_hex_uart((unsigned long)v);
    write_uart("\r\n", 2);
#endif
    v->next = head;
    v->prev = head->prev;
    head->prev->next = v;
    head->prev = v;
}

static struct list_head *remove_head(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->next;
    head->next = head->next->next;
    head->next->prev = head;

    return ptr;
}

static struct list_head *remove_tail(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->prev;
    head->prev = head->prev->prev;
    head->prev->next = head;

    return ptr;
}

static void unlink(struct list_head *node) {
    struct list_head *next, *prev;
    next = node->next;
    prev = node->prev;
    
    next->prev = prev;
    prev->next = next;
}


static void *split_frames(int order, int target_order) {
    /* take the ready to split frame out */
    struct list_head *ptr = remove_head(&bins[order]);

#ifdef DEBUG
    print_uart("split frame: ");
    write_hex_uart((unsigned long)ptr);
    write_uart("\r\n", 2);
#endif
    /* puts splitted frame into bin list */
    for (int i = order; i > target_order; i--) {
        struct list_head *s = (struct list_head *)((char *)ptr + ORDER2SIZE(i-1));
        insert_head(&bins[i-1], s);
        frame[((uintptr_t)s - FRAME_BASE) / PAGE_SIZE] = i - 1;
    }
    frame[((uintptr_t)ptr - FRAME_BASE) / PAGE_SIZE] = target_order | FRAME_INUSE;
    return ptr;
}


/* Assuming total frame size can be divide by biggest bin size */
static void init_buddy() {
    struct list_head *victim;

    for (int i = 0; i < FRAME_BINS; i++) {
        bins[i].next = &bins[i];
        bins[i].prev = &bins[i];
    }
    
    for (uint64_t i = 0; i < FRAME_ARRAY_SIZE; i += 1 << MAX_ORDER) {
        frame[i] = MAX_ORDER;

        victim = (struct list_head *)(FRAME_BASE + i * PAGE_SIZE);
        insert_tail(&bins[MAX_ORDER], victim);

        for (int j = 0; j < (1 << MAX_ORDER); j++) {
            frame[i + j] |= FRAME_FREE;
        }
    }
}

static unsigned pages_to_order(unsigned count) {
    count--;
    count |= count >> 1;
    count |= count >> 2;
    count |= count >> 4;
    count |= count >> 8;
    count |= count >> 16;
    count++;

    return __builtin_ctz(count);
}

static void *alloc_pages(unsigned count) {
    if (!init) {
        init = 1;
        init_buddy();
    }

    int target_order = pages_to_order(count);

    for (int i = target_order; i < FRAME_BINS; i++) {
        if (bins[i].next != &bins[i]) {
            return split_frames(i, target_order);
        }
    }

    /* TODO: OOM handling */
    return NULL;
}

static void set_order(int idx, int order) {
    frame[idx] &= ~FRAME_FLAG_MASK;
    frame[idx] |= order & FRAME_FLAG_MASK;
}

static void free_pages(void *victim) {
    int page_idx = ((uintptr_t)victim - FRAME_BASE) / PAGE_SIZE;
    if (!IS_INUSE(frame[page_idx])) {
        puts_uart("[Warn] Kernel: double free");
        return;
    }
    
    unsigned order = FRAME_ORDER(frame[page_idx]);
    int buddy_page_idx = page_idx ^ (1 << order);
    frame[page_idx] &= ~FRAME_INUSE;

    /* merge frames */
    while (order <= MAX_ORDER &&
           !IS_INUSE(frame[buddy_page_idx]) &&
           order == FRAME_ORDER(frame[buddy_page_idx]))
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

    insert_head(&bins[order], victim);
    set_order(page_idx, order);
}

void *kmalloc(unsigned int size) {
    if (!size || size > FRAME_SIZE_MAX) {
        return NULL;
    }

    unsigned pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
    void *ptr = alloc_pages(pages);

    return ptr;
}

void kfree(void *p) {
    free_pages(p);
}