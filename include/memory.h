#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_START        0x90000
#define PAGE_SIZE           4096
#define MAX_ORDER           9
#define MAX_SIZE_IN_PAGE    (1 << MAX_ORDER)
#define MAX_PAGE_NUMBER     4096

// static void* NULL = 0;

#include "list.h"

struct page
{
    // must be put in the front
    struct list_head list;
    int order;
    int page_number;
    int used;
    void *address;
};

void init_buddy();
struct page *block_allocation(int order);
void block_free(struct page *block);

int find_buddy(int page_number, int order);

#endif