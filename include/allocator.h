#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define PAGE_SIZE 0x1000
#define MAX_BUDDY_ORDER 9 // 2^0 ~ 2^8

enum page_status {
    AVAIL,
    USED,
};

struct list_head {
    struct list_head *next, *prev;
};

struct buddy_head {
    unsigned long nr_free;
    struct list_head head;
};

struct page_t {
    struct list_head list;
    int order;
    unsigned long idx;
    enum page_status status;
};


void mm_init();
void *kmalloc(unsigned long size);
void kfree(void *ptr);

void buddy_init();
void buddy_push(struct buddy_head *buddy, struct list_head *element);
void *buddy_alloc(int order);
void buddy_free(void *mem_addr);
void buddy_remove(struct buddy_head *buddy, struct list_head *element);
struct page_t *buddy_pop(struct buddy_head *buddy, int order);
struct page_t *find_buddy(unsigned long offset, int order);

void page_init();
void mm_init();

#endif
