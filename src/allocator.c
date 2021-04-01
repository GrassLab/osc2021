#include "allocator.h"
#include "cpio.h"
#include "uart.h"

void allocator_init() {
    struct PageInfo *page_info = (struct PageInfo *)MEM_INFO_BASE;
    page_info->page_size = PAGE_SIZE;
    page_info->id = 0;
    uart_printf("ALLOCATOR: init, %d 4k pages", page_info->page_size);
}

void *kmalloc(unsigned long id, unsigned long size) {
    // size need dividing by 4k
    unsigned long alloc_size = align_up(size, 0x1000) / 0x1000;

    // find first more than needed
    struct PageInfo *starter = (struct PageInfo *)MEM_INFO_BASE;
    //struct PageInfo *end = (struct PageInfo *)MEM_INFO_BASE + PAGE_SIZE * sizeof(struct PageInfo);

    // if starter >= end, could not find
    unsigned long offset = 0;
    while(starter->id != 0 || starter->page_size < alloc_size) {
        offset += starter->page_size;
        starter = (struct PageInfo *)((unsigned long)starter + starter->page_size * sizeof(struct PageInfo));

    }

    // use current block
    void *true_mem = (void *)(MEM_BASE + offset * 0x1000);
    starter->id = id;
    starter->real_size = size;
    starter->mem_addr = true_mem;


    // split into least need
    unsigned long s = starter->page_size >> 1;
    struct PageInfo *p=starter;
    while(s >= alloc_size) {
        p = (struct PageInfo *)((unsigned long)starter + s * sizeof(struct PageInfo));
        p->page_size = s;
        p->id = 0;
        p->mem_addr = (void *)(MEM_BASE + (offset + s) * 0x1000);
        starter->page_size = s;
        uart_printf("split: %x and %x\n", starter->mem_addr, p->mem_addr);
        s>>=1;
    }

    uart_printf("ALLOCATOR: alloc %dkb @ 0x%x\n", starter->page_size * 4, true_mem);

    return true_mem;
}

void free(unsigned long id) {
    // find all block belong to id
    struct PageInfo *starter = (struct PageInfo *)MEM_INFO_BASE;
    struct PageInfo *end = (struct PageInfo *)(MEM_INFO_BASE+PAGE_SIZE*sizeof(struct PageInfo));
    struct PageInfo *next;

    while(starter < end) {
        next = (struct PageInfo *)((unsigned long)starter + starter->page_size*sizeof(struct PageInfo));
        if(starter->id == id) {
            starter->id = 0;

            uart_printf("free page %dkb @ 0x%x\n", starter->page_size * 4, starter->mem_addr);

            // merge with next;
            while(next < end && next->id == 0 && next->page_size == starter->page_size) {
                uart_printf("merge pool 0x%x and 0x%x\n", starter->mem_addr, next->mem_addr);
                starter->page_size <<= 1;
                next = (struct PageInfo *)((unsigned long)starter + starter->page_size*sizeof(struct PageInfo));
            }
        }
        starter = next;
    }
}
