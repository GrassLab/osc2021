#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MEM_BASE 0x10000000
#define MEM_MAX_ADDR 0x20000000
#define MEM_INFO_BASE 0x20000000
#define PAGE_SIZE 0x1000

enum page_status {
    AVAIL,
    USED,
};

struct PageInfo {
    void *mem_addr;
    unsigned long real_size;
    unsigned long page_size;
    unsigned long size;
    enum page_status status;
    unsigned long id;
};


void allocator_init();
void *kmalloc(unsigned long size);
void kfree(void *ptr);
void buddy_merge();
unsigned long find_buddy(unsigned long offset, unsigned long size);

#endif
