#include "allocator.h"
#include "cpio.h"
#include "mmio.h"
#include "uart.h"

extern unsigned char __end;
struct PageInfo *page;

void allocator_init() {
    unsigned long first_avail_page = (unsigned long)&__end / PAGE_SIZE + 1;
    unsigned long mmio_base_page = MMIO_BASE / PAGE_SIZE;
    uart_printf("%d\n", mmio_base_page);

    unsigned long bytes = sizeof(struct PageInfo) * mmio_base_page;
    unsigned long ds_page = align_up(bytes, PAGE_SIZE) / PAGE_SIZE;

    void *page_addr = (void *)(first_avail_page * PAGE_SIZE);
    page = (struct PageInfo *) page_addr;
    first_avail_page += ds_page;

    // init buddy system
    unsigned long step = sizeof(unsigned long)*8 - __builtin_clzl(mmio_base_page) - 1;
    step = 1<<step;
    unsigned long idx = 0;
    while(step) {
        if(idx+step <= mmio_base_page) {
            page[idx].size = step;
            page[idx].status = AVAIL;
            idx += step;
        }
        step >>= 1;
    }
    

    // alloc kernel address
    uart_printf("%d\n", mmio_base_page);
    kmalloc(first_avail_page * PAGE_SIZE);
}

void *kmalloc(unsigned long size) {
    // size need dividing by 4k
    unsigned long page_need = align_up(size, PAGE_SIZE) / PAGE_SIZE;

    // find first more than needed
    //struct PageInfo *end = (struct PageInfo *)MEM_INFO_BASE + PAGE_SIZE * sizeof(struct PageInfo);

    // if starter >= end, could not find
    unsigned long offset = 0;
    while(page[offset].status == USED || page[offset].size < page_need) {
        offset += page[offset].size;
    }

    // split into least need
    unsigned long s = page[offset].size >> 1;
    while(s >= page_need) {
        page[offset].size = s;
        page[offset+s].size = s;
        page[offset+s].status = AVAIL;
        s>>=1;
    }
    page[offset].status = USED;

    void *mem = (void *)(offset*PAGE_SIZE);

    uart_printf("ALLOCATOR: alloc %d pages @ page %d - %d\n", page_need, offset, offset+page[offset].size);

    return mem;
}

void kfree(void *ptr) {
    unsigned long page_offset = (unsigned long)ptr / PAGE_SIZE;

    page[page_offset].status = AVAIL;
    buddy_merge(page_offset);
}

void buddy_merge(unsigned long offset) {
    unsigned long buddy_offset = find_buddy(offset, page[offset].size);
    if(page[buddy_offset].status == AVAIL && page[buddy_offset].size == page[offset].size) {
        if(buddy_offset < offset) {
            page[buddy_offset].size = page[offset].size * 2;
            uart_printf("merge page %d and %d to size %d\n", buddy_offset, offset, page[buddy_offset].size);
            buddy_merge(buddy_offset);
        }
        else {
            page[offset].size = page[offset].size * 2;
            uart_printf("merge page %d and %d to size %d\n", offset, buddy_offset, page[offset].size);
            buddy_merge(offset);
        }
    }
}

unsigned long find_buddy(unsigned long offset, unsigned long size) {
    unsigned long buddy_offset = offset ^ size;
    return buddy_offset;
}
