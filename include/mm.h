#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_START            0xA0000

#define PAGE_SHIFT              12
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define MAX_BUDDY_ORDER         9
#define MAX_BLOCK_SIZE          (1 << MAX_BUDDY_ORDER)
#define MAX_PAGE_NUMBER         4096

#define MIN_OBJECT_ORDER        4
#define MAX_OBJECT_ORDER        11
#define MIN_OBJECT_SIZE         (1 << MIN_OBJECT_ORDER)
#define MAX_OBJECT_SIZE         (1 << MAX_OBJECT_ORDER)
#define MAX_ALLOCATOR_NUMBER    MAX_OBJECT_ORDER - MIN_OBJECT_ORDER + 1

#define NULL ((void *)0)

#include "list.h"

struct page
{
    struct list_head list;                          // must be put in the front

    int order;

    void *first_free;
    struct object_allocator *allocator;
    int object_count;                               // how many objects this page stores currently

    int page_number;
    int used;
    void *start_address;
};

struct object_allocator
{
    struct page *current_page;
    // struct page *preserved_empty;
    struct list_head partial;
    struct list_head full;
    int max_object_count;                           // how many objects pages controlled by this allocator can store
    int object_size;
};

void init_buddy();
void init_object_allocator();
void init_memory();

struct page *block_allocation(int order);
void block_free(struct page *block);
void *object_allocation(int token);
void object_free(void *object);

void *km_allocation(int size);
void km_free(void *address);

int find_buddy(int page_number, int order);

#endif