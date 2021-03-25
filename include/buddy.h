#ifndef __BUDDY_H__
#define __BUDDY_H__
#include "system.h"

#define PAGE_SIZE (KB << 2)
#define MEMORY_START 0x10000000
#define MEMORY_END (MEMORY_START + PAGE_SIZE * 1024)//0x20000000
#define FRAME_ARRAY_LENGTH ((MEMORY_END - MEMORY_START) / PAGE_SIZE)
#define MEMORY_LIST_LENGTH 12

struct memFrame {
    long int size; // exponential
    char* addr;
    struct memFrame* next;
};
typedef struct memFrame memFrame;
void buddy_init();
void buddy_new(int);
void buddy_free(char*);
void buddy_merge(int);
void __show_buddy_system();
void __release_block(memFrame*);
void buddy_test1();
memFrame* buddy_alloc(unsigned long int);
int __fit_size_exp(unsigned long int);

#endif
