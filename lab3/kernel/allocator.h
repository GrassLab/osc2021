#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <stdint.h>
#include "miniuart.h"
#include "string.h"
#include "printf.h"

#define BUDDY_START_ADDR 0x00000000
#define PAGE_SIZE 4096
#define ROUNDUP_MUL16(num) ((num + 0xF) & ~0xF)

//split 4K into 251*16byte chunk
typedef struct __Dynamic_Mem_Page{
    struct __Dynamic_Mem_Page *next;
    int16_t state[32];  //0 == free, 1 == start, 2 == end
    char data[251][16];
} Dynamic_Mem_Page;

//sizeof(Page) <= PAGE_SIZE
typedef struct __Page {
    struct __Page *next;
    size_t index;
} Page;

typedef struct {
    uint16_t *page_frame_bit;  //1 == free, 2 == start, 3 == end
    Page **free_page_list;
    Dynamic_Mem_Page *malloc_page;
} Buddy_System;

static size_t log2ceil(unsigned int num);
static void set_2bitarray(uint16_t *array, size_t index, uint8_t value);
static uint8_t read_2bitarray(uint16_t *array, size_t index);
void buddy_init();
static void palloc_index(size_t index);
static void pfree_index(size_t index);
void *palloc(size_t num_of_page);
void pfree(void *p);
static void push_page_list(void *p, size_t index, Page **list);
void show_page_frame_bit();
void show_free_page_list();
void *malloc(size_t num_of_byte);
void mfree(void *p);

#endif
