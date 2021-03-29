#include "mm.h"
#include "loader.h"
#include "data_type.h"
#include "string.h"

// TODO
#include "uart.h"

#define mask 0xffffffffffffffff
#define page_start(addr) ((addr) & page_mask)
#define page_end(addr) ((mask - page_size + 1) & (addr + page_size - 1))
struct startup_mm_list { u64 addr_start, addr_end;
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
    u64 memory_size = 0x100000000;
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

    if (ptr && ptr->addr_end < memory_size) {
        struct startup_mm_list *tmp = (struct startup_mm_list *)startup_node_ptr;
        startup_node_ptr += sizeof(struct startup_mm_list);
        tmp->addr_start = ptr->addr_end;
        tmp->addr_end = memory_size;
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

    /* merge overlapping memory */
    for (struct startup_mm_list *ptr= *head; ptr && ptr->next;) {
        if (ptr->addr_end >= ptr->next->addr_start) {
            /* merge nodes */
            if (ptr->addr_end < ptr->next->addr_end)
                ptr->addr_end = ptr->next->addr_end;
            ptr->next = ptr->next->next;
        }
        else
            ptr = ptr->next;
    }
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

/* buddy system */
#define is_page_used(val) ((val) & 0b10000000)
#define is_page_head(val) ((val) & 0b01000000)
#define get_page_size(val) (0x1000 << ((val) & 0b00011111))
#define set_page_used(val) val |= 0b10000000
#define clear_page_used(val) val &= 0b01111111
#define set_page_head(val) val |= 0b01000000
#define clear_page_head(val) val &= 0b10111111
#define bucket_size 21 /* 4KB ~ 4GB */
typedef struct bs_entry_table {
    u8 *bs_info_table;
    u64 memory_start;
    u64 memory_end;
    u64 memory_size;
} BS_entry;

typedef struct bs_frame_list {
    void *addr;
    BS_entry *table;
    u64 offset;
    struct bs_frame_list *next;
} BS_frame;

typedef struct bs_frame_head {
    BS_frame *head;
    u64 num;
} BS_frame_head;

BS_entry *bs_table = NULL;
u64 *bs_table_size = NULL;
BS_frame_head *bs_frame_bucket = NULL;

void buddy_system_init () {
    startup_used_list_reorder();
    startup_add_unused_memory();
    bs_table_size = (u64 *) startup_malloc(sizeof(u64 *));
    startup_aligned(0x10);

    *bs_table_size = 0;
    for (struct startup_mm_list *ptr= startup_freed_head; ptr; ptr = ptr->next)
        ++*bs_table_size;

    bs_table = (BS_entry *) startup_malloc(sizeof(BS_entry) *
            (*bs_table_size));
    /* 4KB ~ 4GB */
    bs_frame_bucket = (BS_frame_head *) startup_malloc(sizeof(BS_frame_head)
            * bucket_size);
    for (int i = 0; i < bucket_size; i++) {
        bs_frame_bucket[i].head = (BS_frame *) 0x0;
        bs_frame_bucket[i].num = 0;
    }
    startup_aligned(page_size);

    int i = 0;
    for (struct startup_mm_list *ptr= startup_freed_head; ptr; i++) {
        u64 chunck_size = (ptr->addr_end - ptr->addr_start) >> 12;
        bs_table[i].bs_info_table = (u8 *) ptr->addr_start;
        startup_malloc(chunck_size);
        startup_aligned(page_size);
        bs_table[i].memory_end = ptr->addr_end;
        bs_table[i].memory_size = (ptr->addr_end - ptr->addr_start) >> 12;
        bs_table[i].memory_start = ptr->addr_start;

        mem_set((char *)bs_table[i].bs_info_table, 0, bs_table[i].memory_size);

        u8 *info_table = bs_table[i].bs_info_table;
        u64 size = bs_table[i].memory_size - 1;
        set_page_head(info_table[0]);
        set_page_used(info_table[0]);

        u64 offset = 1;
        for (u64 j = 0; j < bucket_size; j++) {
            u8 index = (20 - j);
            u64 page = 1 << (index);
            while (size > page) {
                size -= page;
                info_table[offset] = index;
                if (page > 0)
                    info_table[offset + page - 1] = index;
                set_page_head(info_table[offset]);

                /* push a node into linked-list buckets */
                BS_frame *tmp = startup_malloc(sizeof(BS_frame));
                tmp->next = bs_frame_bucket[index].head;
                tmp->addr = (void *) (bs_table[i].memory_start + page_size * offset);
                tmp->offset = offset;
                tmp->table = &bs_table[i];
                bs_frame_bucket[index].num += 1;
                bs_frame_bucket[index].head = tmp;

                offset += page;
            }
        }

        /* remove the node from startup allocator */
        startup_freed_head = ptr->next;
        ptr = ptr->next;
    }
}

void buddy_system_show_buckets () {
    for (u64 i = 0; i < bucket_size; i++) {
        uart_sendi(i);
        uart_send(": ");
        uart_sendi(bs_frame_bucket[i].num);
        uart_send("\r\n");
    }
}

void buddy_system_show_entry_table () {
    for (u64 i = 0; i < *bs_table_size; i++) {
        uart_send("info table: ");
        uart_sendh((u64)bs_table[i].bs_info_table);

        uart_send("\r\nmemory start: ");
        uart_sendh(bs_table[i].memory_start);

        uart_send("\r\nmemory end: ");
        uart_sendh(bs_table[i].memory_end);

        uart_send("\r\n\r\n");
    }
}

/* size should be bigger than 4KB */
void *bs_malloc (u64 size) {
    if (size & 0xfff) return NULL;

    u64 index = 0;
    for (u64 i = size >> 13; i; i/=2) index++;

    void *addr = NULL;
    u64 select = index;
    BS_frame *frame = NULL;
    for (; select < bucket_size; select++) {
        if (bs_frame_bucket[select].head) {
            frame = bs_frame_bucket[select].head;
            bs_frame_bucket[select].head = bs_frame_bucket[select].head->next;
            bs_frame_bucket[select].num -= 1;
            break;
        }
    }
    return addr;
}
