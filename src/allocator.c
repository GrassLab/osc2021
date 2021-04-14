#include "allocator.h"
#include "cpio.h"
#include "mmio.h"
#include "typedef.h"
#include "uart.h"
#include "mbox.h"

extern unsigned char __end;


struct buddy_head buddy_order[MAX_BUDDY_ORDER];
struct page_t *page;

void buddy_init() {
    for(int i=0;i<MAX_BUDDY_ORDER;i++) {
        buddy_order[i].nr_free = 0;
        buddy_order[i].head.next = &buddy_order[i].head;
        buddy_order[i].head.prev = &buddy_order[i].head;
    }
}

void buddy_push(struct buddy_head *buddy, struct list_head *element) {
    buddy->nr_free++;
    element->next = &(buddy->head);
    element->prev = buddy->head.prev;
    buddy->head.prev->next = element;
    buddy->head.prev = element;
}

struct page_t *buddy_pop(struct buddy_head *buddy, int order) {
    if(buddy->head.next == &buddy->head) return NULL;
    buddy->nr_free--;
    // get target
    struct list_head *target = buddy->head.next;

    uart_printf("pop head: %x\n", target);

    // remove from list;
    target->next->prev = target->prev;
    target->prev->next = target->next;
    target->next = NULL;
    target->prev = NULL;

    // release redundant page
    struct page_t *target_page = (struct page_t *)target;
    int cur_order = target_page->order;

    while(cur_order > order) {
        uart_printf("split order %d to %d\n", cur_order, cur_order-1);
        cur_order--;
        struct page_t *left = target_page;
        struct page_t *right = target_page + (1<<cur_order);
        left->order = cur_order;
        right->order = cur_order;
        buddy_push(&buddy_order[cur_order], &(right->list));
    }

    for(int i=0;i<(1<<order);i++) {
        target_page[i].status = USED;
    }
    return target_page;
}

void buddy_remove(struct buddy_head *buddy, struct list_head *element) {
    buddy->nr_free--;
    element->prev->next = element->next;
    element->next->prev = element->prev;
    element->prev = NULL;
    element->next = NULL;
}

void *buddy_alloc(int order) {
    // find free space
    for(int i=order;i<MAX_BUDDY_ORDER;i++) {
        if(buddy_order[i].nr_free>0) {
            struct page_t *target = buddy_pop(&buddy_order[i], order);
            return (void *)(target->idx * PAGE_SIZE);
        }
    }
    return NULL;
}

void buddy_free(void *mem_addr) {
    unsigned long page_idx = (unsigned long)mem_addr / PAGE_SIZE;
    struct page_t *p = &page[page_idx];
    int order = p->order;
    struct page_t *buddy_p = find_buddy(page_idx, order);


    // merge buddy
    while(buddy_p->status == AVAIL && buddy_p->order == order) {
        buddy_remove(&buddy_order[order], &(buddy_p->list));

        order++;
        if(p<buddy_p) {
            page_idx = (unsigned long)p / PAGE_SIZE;
            buddy_p->order = -1;
        }
        else {
            page_idx = (unsigned long)buddy_p / PAGE_SIZE;
            p->order = -1;
            p = buddy_p;
        }
        buddy_p = find_buddy(page_idx, order);
    }

    for(int i=0;i<(1<<order);i++) {
        (p+i)->status = AVAIL;
        p->order = order;
        buddy_push(&buddy_order[order], &(p->list));
    }
}

struct page_t *find_buddy(unsigned long offset, int order) {
    unsigned long buddy_offset = offset ^ (1<<order);
    return &page[buddy_offset];
}

void page_init() {
    uart_printf("end: %x\n", arm_memory_end);
    unsigned long first_avail_page = (unsigned long)&__end / PAGE_SIZE + 1;
    unsigned long end_page = arm_memory_end / PAGE_SIZE;
    uart_printf("%d\n", end_page);

    unsigned long bytes = sizeof(struct page_t) * end_page;
    unsigned long ds_page = align_up(bytes, PAGE_SIZE) / PAGE_SIZE;

    void *page_addr = (void *)(first_avail_page * PAGE_SIZE);
    page = (struct page_t *) page_addr;
    first_avail_page += ds_page;

    for(int i=0;i<first_avail_page;i++) {
        page[i].status = USED;
        page[i].idx = i;
    }


    int remainder = first_avail_page % (1<< (MAX_BUDDY_ORDER-1));
    int order = MAX_BUDDY_ORDER - 1;
    int idx = first_avail_page + (1<<(MAX_BUDDY_ORDER-1)) + remainder;
    int counter = 0;
    while(idx<end_page) {
        if(counter) {
            page[idx].status = AVAIL;
            page[idx].order = -1;
            page[idx].list.next = NULL;
            page[idx].list.prev = NULL;
            page[idx].idx = idx;
            counter--;
            idx++;
        }
        else if(idx + (1<<order)-1 < end_page) {
            uart_printf("order: %d, idx: %d, addr: %x, phy: %x\n", order, idx, &page[idx].list, idx*PAGE_SIZE);
            page[idx].status = AVAIL;
            page[idx].order = order;
            page[idx].idx = idx;
            buddy_push(&(buddy_order[order]), &(page[idx].list));
            counter = (1<<order) - 1;
            idx++;
        }
        else {
            order--;
        }
    }
}

void mm_init() {
    buddy_init();
    page_init();
}

void *kmalloc(unsigned long size) {
    // find order
    int order;
    for(int i=0;i<MAX_BUDDY_ORDER;i++) {
        if(size <= (unsigned long)(1<<i)*PAGE_SIZE) {
            order = i;
            break;
        }
    }
    return buddy_alloc(order);
}

void kfree(void *ptr) {
    buddy_free(ptr);
}

