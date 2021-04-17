#ifndef _LCD_BUDDY_H_
#define _LCD_BUDDY_H_

#include "type.h"

#define FREE_FRAME_ALLOCATABLE     -1
#define USED_FRAME_UNALLOCATABLE   -2

#define BASE_ADDRESS               0x10000000
#define FRAME_SIZE                 0x1000
#define FRAME_NUMBERS              0x10000

#define FRAME_MAX_ORDER            17

struct buddy_frame {
    int index;
    int order;
    int used_order;
    uint64_t start_address;
    struct buddy_frame *next;
};

void buddy_init();

/* 
 * This function allocates frame with buddy memory allocation.
 * If there is enough memory to allocate, return an allocated address.
 * Otherwise, return NULL.
 */
void *allocate_frame(int required_size_in_kbyte);

void freelist_deletion(int order, struct buddy_frame *frame);
void freelist_insertion(int order, struct buddy_frame *frame);

/*
 * This function releases frame by required size in KB.
 * If there is no such frame to release or input error, return -1.
 * Otherwise, return freed frame's index.
 */
int free_frame_by_size(int freed_size_in_kbyte);

/* 
 * This function releases frame by specific index.
 * If the index is freeable, return 0.
 * Otherwise, return -1;
 */
int free_frame_by_index(int freed_index);

void print_available_memory_with_uart();
void print_used_memory_with_uart();

#endif