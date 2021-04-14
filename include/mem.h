#ifndef MEM_H
#define MEM_H

#include <stddef.h>

#define BUDDY_MAX_ORD 16    // 4kB ~ 128MB
#define MEM_MAX_RESERVE 16  // max reserve slot
#define PAGE_SIZE 4096
#define PAGE_SIZE_CTZ 12
#define MEM_PAD 16

#define RESERVE_NOT_ALIGN -1
#define RESERVE_SLOT_FULL -2
#define RESERVE_COLLISION -3

extern unsigned long mem_size;

// allocate continuous mem with size align {MEM_PAD}
void *kmalloc(unsigned long size);
// free memory allocate by kmalloc, reserve_mem, reserve_alloc
void kfree(void *ptr);
// initialize kmalloc before using
void init_kmalloc();
// reserve memory using startup allocator align {PAGE_SIZE}
int reserve_mem(void *addr, unsigned long size);
// allocate memory using startup allocator align {PAGE_SIZE}
void *reserve_alloc(unsigned long size);

#endif