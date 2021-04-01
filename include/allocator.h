#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#define MEM_BASE 0x10000000
#define MEM_MAX_ADDR 0x20000000
#define MEM_INFO_BASE 0x20000000
#define PAGE_SIZE ((MEM_MAX_ADDR - MEM_BASE) / 0x1000)

struct PageInfo {
    void *mem_addr;
    unsigned long real_size;
    unsigned long page_size;
    unsigned long id;
};


void allocator_init();
void *kmalloc(unsigned long id, unsigned long size);
void free(unsigned long pid);

#endif
