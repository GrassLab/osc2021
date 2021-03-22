#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#define BUDDY_MAX 16    // 4kB ~ 128MB
#define STARTUP_MAX 16  // max reserve slot
#define PAGE_SIZE 4096
#define PAGE_SIZE_CTZ 12
// #define SMALLBIN_SiZE 14   // 0x20 ~ 0xf0
// #define MIDDLEBIN_SIZE 15  // 0x100 ~ 0xf00
// #define INIT_HEAP_START 0x20000
// #define INIT_HEAP_SIZE 0x40000

extern unsigned long long mem_size;

void *kmalloc(unsigned long size);
void kfree(void *ptr);

int reserve_mem(void *addr, unsigned long size);
void init_buddy(char *ps);
// void init_heap(void *heap_start, size_t heap_size);

void *alloc_page(size_t size);
// void *alloc_heap(size_t size);

void free_page(void *ptr);
// void free_heap(void *ptr);
void free_reserve(void *ptr);

void log_buddy();
#endif