#include "mm.h"
#include "loader.h"
#include "data_type.h"

// TODO
#include "uart.h"

#define mask 0xffffffffffffffff
#define page_start(addr) ((addr) & page_mask)
#define page_end(addr) ((mask - page_size + 1) & (addr + page_size - 1))

struct startup_mm_list {
    u64 addr_start, addr_end;
    struct startup_mm_list *next;
};

struct startup_mm_list *startup_allocated_head = 0,
                              *startup_freed_head = 0;
void *startup_node_ptr = 0;

void startup_aligned (u64 size) {
    u64 new_mask = mask - size + 1;
    startup_freed_head->addr_start =
        (startup_freed_head->addr_start + size - 1) & new_mask;
}

void *startup_malloc (u64 size) {
    void *mm = NULL;
    for (struct startup_mm_list **list = &startup_freed_head; *list;
            list = &(*list)->next) {
        if ((*list)->addr_start + size <= (*list)->addr_end) {
            mm = (void *)(*list)->addr_start;
            (*list)->addr_start += size;
            if ((*list)->addr_start == (*list)->addr_end)
                *list = (*list)->next;
            return mm;
        }
    }
    return mm;
}

void startup_allocator_init () {
    /* find an empty page */
    u64 kernel_end = page_end(boot_info.kernel_addr + boot_info.kernel_size);
    u64 bootloader_end = page_end(boot_info.bootloader_addr + boot_info.bootloader_size);
    u64 stack_start = page_start(boot_info.stack_base - boot_info.stack_size);
    startup_node_ptr = (void *) (kernel_end > bootloader_end ? kernel_end : bootloader_end);
    boot_info.startup_allocator_addr = (u64)startup_node_ptr;

    /* memory between kernel and kernel stack */
    startup_freed_head = (struct startup_mm_list *)startup_node_ptr;
    startup_node_ptr += sizeof(struct startup_mm_list);
    startup_freed_head->addr_start = page_end((u64)startup_node_ptr);
    startup_freed_head->addr_end = stack_start;
    startup_freed_head->next = NULL;

    /* lock used memory */
    startup_lock_memory(boot_info.kernel_addr, boot_info.stack_base);
    startup_lock_memory(0x0, 0x1000);
}

void startup_lock_memory (u64 start, u64 end) {
    struct startup_mm_list *ptr = (struct startup_mm_list *)startup_node_ptr;
    startup_node_ptr += sizeof(struct startup_mm_list);
    ptr->addr_start = page_start(start);
    ptr->addr_end = page_end(end);
    ptr->next = startup_allocated_head;
    startup_allocated_head = ptr;
}

void startup_add_unused_memory () {
    /* 4 GB */
    u64 memory_size = 0x4000000000;
    struct startup_mm_list *ptr;

    for (ptr = startup_allocated_head; ptr && ptr->next; ptr = ptr->next) {
        if (ptr->addr_start > memory_size || ptr->next->addr_end > memory_size)
            return;
        struct startup_mm_list *tmp = (struct startup_mm_list *)startup_node_ptr;
        startup_node_ptr += sizeof(struct startup_mm_list);
        tmp->addr_start = ptr->addr_end;
        tmp->addr_end = ptr->next->addr_start;
        tmp->next = startup_freed_head;
        startup_freed_head = tmp;
    }
}

void startup_used_list_reorder () {
    char is_change = 1;
    struct startup_mm_list **head = &startup_allocated_head;
    while (is_change) {
        is_change = 0;
        for (struct startup_mm_list **ptr = head; *ptr && (*ptr)->next;
                ptr = &(*ptr)->next) {
            /* swap nodes */
            if ((*ptr)->addr_start > (*ptr)->next->addr_start) {
                struct startup_mm_list *tmp = *ptr;
                *ptr = tmp->next;
                tmp->next = (*ptr)->next;
                (*ptr)->next = tmp;
                is_change = 1;
            }
        }
    }

    startup_add_unused_memory();
}

void show_list () {
    for (struct startup_mm_list *ptr = startup_freed_head; ptr;
            ptr = ptr->next) {
        uart_sendh(ptr->addr_start);
        uart_send(" ");
        uart_sendh(ptr->addr_end);
        uart_send("\r\n");
    }
}
