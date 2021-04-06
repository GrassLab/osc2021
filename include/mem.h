#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#define BUDDY_MAX 16    // 4kB ~ 128MB
#define STARTUP_MAX 16  // max reserve slot
#define PAGE_SIZE 4096
#define PAGE_SIZE_CTZ 12

extern unsigned long long mem_size;

void *kmalloc(unsigned long size);
void kfree(void *ptr);

void init_buddy(char *ps);
void init_slab();
void *register_slab(size_t size);

void *alloc_page(size_t size);
int reserve_mem(void *addr, unsigned long size);
void *alloc_slab(void *slab_tok);

void free_page(void *ptr);
void free_reserve(void *ptr);
void free_slab(void *ptr, void *slab);
void free_unknow_slab(void *ptr);

void log_buddy();
void check_buddy_stat();
void check_slab();
#endif