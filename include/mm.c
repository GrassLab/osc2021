#include "mm.h"
#include "loader.h"
#include "data_type.h"
#include "string.h"
#include "time.h"

// TODO: remove uart
#include "uart.h"

#define mask 0xffffffffffffffff
#define m_size_mask 0xfffffffffffffff0
#define m_used_bit 0x0000000000000001
#define m_last_chunk_bit 0x0000000000000002
#define page_start(addr) ((addr) & page_mask)
#define page_end(addr) ((mask - page_size + 1) & (addr + page_size - 1))
#define malloc_struct_size (sizeof(malloc_struct))

/* 4 GB */
#define memory_size 0x100000000
void * (*m_malloc) (u64 size);
void (*m_free) (void *addr);

#define log(type, message, start, end) \
    uart_send("["); \
    uart_sendf(get_time()); \
    uart_send("] "); \
    uart_send(type); \
    uart_send(": "); \
    uart_send(message); \
    uart_send(" "); \
    uart_sendh((u64) start); \
    uart_send(" ~ "); \
    uart_sendh((u64) end); \
    uart_send("\r\n"); \

struct startup_mm_list {
    u64 addr_start, addr_end, orig_start;
    struct startup_mm_list *next;
};

typedef struct glibc_malloc_struct {
    u64 prev, info;
    u64 data[];
} malloc_struct;

typedef struct malloc_small_bin_list {
    struct malloc_small_bin_list *next;
} small_bin_list;

typedef struct malloc_large_bin_list {
    struct malloc_large_bin_list *next, *prev;
} large_bin_list;

/* bins */
#define small_bin_size 0x20
small_bin_list small_bin[small_bin_size];
large_bin_list large_bin;

struct startup_mm_list *init_used_head = 0,
                       *init_freed_head = 0,
                       *startup_memory_head = 0;

/* TODO */
void startup_aligned (u64 size) {
    u64 new_mask = mask - size + 1;
    init_freed_head->addr_start =
        (init_freed_head->addr_start + size - 1) & new_mask;
}

void show_malloc_bins () {
    for (u64 i = 0; i < small_bin_size; i++) {
        uart_sendh(i * 0x10 + 0x20);
        uart_send(":");
            for (small_bin_list *ptr = small_bin[i].next; ptr;
                    ptr = ptr->next) {
                uart_send(" -> ");
                uart_sendh((u64)ptr - 0x10);
            }
        uart_send("\r\n");
    }
    uart_send("large bin: ");
    for (large_bin_list *ptr = large_bin.next; ptr;
            ptr = ptr->next) {
        uart_send(" -> ");
        uart_sendh((u64)ptr - 0x10);
    }
    uart_send("\r\n");
}

malloc_struct *search_freed_bins (u64 size) {
    /* search freed bins */
    if (size <= small_bin_size * 0x10 + 0x10 && size >= 0x20) {
        u64 index = (size - 0x20) / 0x10;
        if (small_bin[index].next) {
            small_bin_list *tmp = small_bin[index].next;
            small_bin[index].next = small_bin[index].next->next;

            malloc_struct *chunk = (malloc_struct *)((u64)tmp - malloc_struct_size);
            chunk->info |= m_used_bit;

            return chunk;
        }
    }
    else {
        for (large_bin_list *ptr = large_bin.next; ptr; ptr = ptr->next) {
            malloc_struct *chunk = (malloc_struct *)((u64)ptr - malloc_struct_size);
            /* split chunk and push remain chunk into freed bins */
            if ((chunk->info & m_size_mask) >= size + 0x20) {
                malloc_struct *r_chunk = (malloc_struct *)((u64)chunk + size);
                u64 r_size = (chunk->info & m_size_mask) - size;
                r_chunk->info = r_size | (chunk->info & m_last_chunk_bit);
                startup_free(&r_chunk->data);

                ptr->next->prev = ptr->prev;
                ptr->prev->next = ptr->next;
                chunk->info |= m_used_bit;

                return chunk;
            }
            else if ((chunk->info & m_size_mask) >= size) {
                ptr->next->prev = ptr->prev;
                ptr->prev->next = ptr->next;
                chunk->info |= m_used_bit;

                return chunk;
            }
        }
    }

    return NULL;
}

void *startup_malloc (u64 size) {
    size = 16 + aligned16(size);
    malloc_struct *mm = search_freed_bins(size);

    if (mm) {
        log("startup allocator", "allocate", (u64)mm,
                (u64)mm + (mm->info & m_size_mask));
        return (void *)&mm->data;
    }

    /* allocate a new area */
    for (struct startup_mm_list **list = &startup_memory_head; *list;
            list = &(*list)->next) {
        if ((*list)->addr_start + size <= (*list)->addr_end) {
            mm = (malloc_struct *)(*list)->addr_start;
            (*list)->addr_start += size;
            if ((*list)->addr_start == (*list)->addr_end)
                *list = (*list)->next;
            mm->prev = 0x0;
            mm->info = size | m_used_bit;
            /* demo information */
            log("startup allocator", "allocate", (u64)mm, (u64)mm + size);
            return (void *)&(mm->data);
        }
    }
    return NULL;
}

void pop_free_node (malloc_struct *node) {
    u64 size = node->info & 0xfffffffffffffff0;
    if (size <= small_bin_size * 0x10 + 0x10 && size >= 0x20) {
        u64 index = (size - 0x20) / 0x10;

        for (small_bin_list **ptr = &small_bin[index].next; *ptr;
                ptr = &(*ptr)->next) {
            if ((u64)*ptr - 0x10 == (u64)node) {
                *ptr = (*ptr)->next;
                return;
            }
        }
    }
    else {

    }
}

void startup_free (void *addr) {
    malloc_struct *whole_chunk = (malloc_struct *)((u64)addr - 0x10);
    u64 size = whole_chunk->info & 0xfffffffffffffff0;
    whole_chunk->info &= (~m_used_bit);    /* clear used bit */
    /* small bins */
    if (size <= small_bin_size * 0x10 + 0x10 && size >= 0x20) {
        small_bin_list *ptr = (small_bin_list *)&whole_chunk->data;
        u64 index = (size - 0x20) / 0x10;
        ptr->next = small_bin[index].next;
        small_bin[index].next = ptr;
    }
    /* large bins */
    else {
        large_bin_list *ptr = (large_bin_list *)&whole_chunk->data;
        ptr->next = large_bin.next;
        ptr->prev = &large_bin;
        large_bin.next->prev = ptr;
        large_bin.next = ptr;
    }

    /* insert information into the next block */
    if (!(whole_chunk->info & m_last_chunk_bit)) {
        malloc_struct *nextb = (malloc_struct *)((u64)whole_chunk + size);
        nextb->prev = (u64)whole_chunk;
    }

    /* demo information */
    log("startup allocator", "free", (u64)whole_chunk, (u64)whole_chunk + size);
}

void startup_allocator_init () {
    m_malloc = startup_malloc;
    m_free = startup_free;

    /* initialize freed bins */
    for (u32 i = 0; i < small_bin_size; i++)
        small_bin[i].next = NULL;
    large_bin.next = NULL;
    large_bin.prev = NULL;

    /* find an empty page */
    void *startup_node_ptr = 0;
    u64 kernel_end = page_end(boot_info.kernel_addr + boot_info.kernel_size);
    u64 bootloader_end = page_end(boot_info.bootloader_addr + boot_info.bootloader_size);
    u64 stack_start = page_start(boot_info.stack_base - boot_info.stack_size);
    startup_node_ptr = (void *) (kernel_end > bootloader_end ? kernel_end : bootloader_end);
    boot_info.startup_allocator_addr = (u64)startup_node_ptr;

    /* memory between kernel and kernel stack */
    startup_memory_head = (struct startup_mm_list *)startup_node_ptr;
    startup_node_ptr += sizeof(struct startup_mm_list);
    startup_memory_head->addr_start = page_end((u64)startup_node_ptr);
    startup_memory_head->orig_start = page_end((u64)startup_node_ptr);
    startup_memory_head->addr_end = stack_start;
    startup_memory_head->next = NULL;

    /* lock used memory */
    startup_lock_memory(boot_info.kernel_addr, boot_info.stack_base);
    startup_lock_memory(0x0, 0x1000);
}

void startup_lock_memory (u64 start, u64 end) {
    struct startup_mm_list *ptr = (struct startup_mm_list *)
        startup_malloc(sizeof(struct startup_mm_list));
    ptr->addr_start = page_start(start);
    ptr->addr_end = page_end(end);
    ptr->next = init_used_head;
    init_used_head = ptr;
}

void init_add_unused_memory () {
    struct startup_mm_list *ptr;

    for (ptr = init_used_head; ptr && ptr->next; ptr = ptr->next) {
        if (ptr->addr_start > memory_size || ptr->next->addr_end > memory_size)
            return;
        struct startup_mm_list *tmp = (struct startup_mm_list *)
            startup_malloc(sizeof(struct startup_mm_list));
        tmp->addr_start = ptr->addr_end;
        tmp->addr_end = ptr->next->addr_start;
        tmp->next = init_freed_head;
        init_freed_head = tmp;
    }

    if (ptr && ptr->addr_end < memory_size) {
        struct startup_mm_list *tmp = (struct startup_mm_list *)
            startup_malloc(sizeof(struct startup_mm_list));
        tmp->addr_start = ptr->addr_end;
        tmp->addr_end = memory_size;
        tmp->next = init_freed_head;
        init_freed_head = tmp;
    }
}

void init_used_list_reorder () {
    char is_change = 1;
    struct startup_mm_list **head = &init_used_head;
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
            struct startup_mm_list *tmp = ptr->next;
            /* merge nodes */
            if (ptr->addr_end < ptr->next->addr_end)
                ptr->addr_end = ptr->next->addr_end;
            ptr->next = ptr->next->next;
            startup_free(tmp);
        }
        else
            ptr = ptr->next;
    }
}

void show_list () {
    //for (struct startup_mm_list *ptr = init_freed_head; ptr;
    for (struct startup_mm_list *ptr = init_used_head; ptr;
            ptr = ptr->next) {
        uart_sendh(ptr->addr_start);
        uart_send(" ");
        uart_sendh(ptr->addr_end);
        uart_send("\r\n");
    }
}

/* buddy system */
#define bucket_size 23 /* 4KB ~ 4GB */
#define bs_used_bit 0b10000000
#define bs_size_bit 0b00111111
#define bs_get_bucket_index(off) (((u64)bs_table_addr[off] & bs_size_bit) - 1)
#define bs_get_page_size(off) ((1 << bs_get_bucket_index(off)) * page_size)
#define bs_get_val(off) ((u64)bs_table_addr[off])

typedef struct bs_frame_list {
    void *addr;
    u64 size;
    struct bs_frame_list *next;
} BS_frame;

typedef struct bs_frame_head { BS_frame *head;
    u64 num;
} BS_frame_head;

u64 bs_table_size = 0;
u8 *bs_table_addr = NULL;
BS_frame_head *bs_frame_bucket = NULL;

BS_frame *bs_pop_bucket (void *addr) {
    u64 offset = (u64)addr / page_size;
    u64 index = bs_get_bucket_index(offset);

    for (BS_frame **ptr = &bs_frame_bucket[index].head; *ptr;
            ptr = &(*ptr)->next) {
        if ((*ptr)->addr == addr) {
            BS_frame *tmp = *ptr;
            *ptr = (*ptr)->next;
            bs_frame_bucket[index].num -= 1;
            return tmp;
        }
    }
    return NULL;
}

void bs_table_set (u64 start, u64 end, u8 is_used) {
    u64 size = (end - start) / page_size;
    u64 offset = start / page_size;
    u64 bottom = offset + size;

    u64 order = 1;
    for (u64 i = 0; i <= bucket_size; i++) {
        if (offset + order > bottom)
            break;
        if (offset % (order * 2)) {
            u8 usedb = 0;
            if (is_used)
                usedb |= bs_used_bit;
            else {
                BS_frame *frame = m_malloc(sizeof(BS_frame));
                frame->addr = (void *)(offset * page_size);
                frame->size = order * page_size;
                frame->next = bs_frame_bucket[i].head;
                bs_frame_bucket[i].head = frame;
                bs_frame_bucket[i].num += 1;
            }
            bs_table_addr[offset] = (i + 1) | usedb;
            offset += order;
        }
        order *= 2;
    }

    order = 1 << (bucket_size - 1);
    for (long i = bucket_size - 1; i >= 0; i--) {
        if (offset + order <= bottom) {
            u8 usedb = 0;
            if (is_used)
                usedb |= bs_used_bit;
            else {
                BS_frame *frame = m_malloc(sizeof(BS_frame));
                frame->addr = (void *)(offset * page_size);
                frame->size = order * page_size;
                frame->next = bs_frame_bucket[i].head;
                bs_frame_bucket[i].head = frame;
                bs_frame_bucket[i].num += 1;
            }
            bs_table_addr[offset] = (i + 1) | usedb;
            offset += order;
        }
        order /= 2;
    }
}

/* index of bucket lists */
u8 bs_page_index (u64 size) {
    if (size < page_size)
        return 0;
    u8 i = 0;
    size /= (2 * page_size);
    for (; size; i++) size /= 2;

    return i;
}

void buddy_system_init () {
    init_used_list_reorder();
    init_add_unused_memory();

    /* find proper space to store buddy system table */
    struct startup_mm_list *ptr = init_freed_head;
    for (; ptr; ptr = ptr->next)
        if (ptr->addr_end - ptr->addr_start > bs_table_size)
            break;

    if (!ptr) {
        uart_send("buddy system intialization failed: no memory space to create table\r\n");
        return;
    }
    boot_info.buddy_system_addr = ptr->addr_start;
    boot_info.buddy_system_size = bs_table_size;
    bs_table_addr = (u8 *)ptr->addr_start;
    ptr->addr_start += bs_table_size;
    mem_set((char *)bs_table_addr, 0, bs_table_size);

    /* create unused memory list */
    bs_frame_bucket = (BS_frame_head *) m_malloc(sizeof(BS_frame_head)
            * bucket_size);
    for (u8 i = 0; i < bucket_size; i++) {
        bs_frame_bucket[i].head = NULL;
        bs_frame_bucket[i].num = 0;
    }

    /* record used pages */
    for (struct startup_mm_list *ptr = init_used_head; ptr; ptr = ptr->next) {
        bs_table_set(ptr->addr_start, ptr->addr_end, 1);
    }

    /* record unused pages */
    for (struct startup_mm_list *ptr = init_freed_head; ptr; ptr = ptr->next) {
        bs_table_set(ptr->addr_start, ptr->addr_end, 0);
    }
}

void buddy_system_show_buckets () {
    for (u64 i = 0; i < bucket_size; i++) {
        uart_sendi(i);
        uart_send("(");
        uart_sendi(bs_frame_bucket[i].num);
        uart_send("):");
        for (BS_frame *ptr = bs_frame_bucket[i].head; ptr; ptr = ptr->next) {
            uart_send(" -> ");
            uart_sendh((u64)ptr->addr);
        }
        uart_send("\r\n");
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

    addr = frame->addr;

    if (select > index) {
        u64 remained_chunck_start = 0, remained_chunck_end = 0;
        remained_chunck_start = (u64)frame->addr + size;
        remained_chunck_end = (u64)frame->addr + frame->size;
        bs_table_set(remained_chunck_start, remained_chunck_end, 0);
    }
    bs_table_set((u64)frame->addr, (u64)frame->addr + size, 1);
    startup_free(frame);

    /* demo information */
    log("buddy system", "allocate", (u64)addr, (u64)addr + size);

    return addr;
}

/* if all pages in the range are unused, pop them all from buckets */
int bs_scann_table (u64 ptr, u64 size) {
    u64 buf = 0;
    for (u64 tmp = ptr; buf < size;) {
        if (!(bs_get_val(tmp) & bs_size_bit)) {
            uart_send("Error: page table is wrong\r\n");
            return 0;
        }

        if (bs_get_val(tmp) & bs_used_bit) {
            return 0;
        }

        u64 val = bs_get_page_size(tmp) / page_size;
        buf += val;
        tmp += val;
    }

    if (size != buf)
        return 0;

    /* pop all nodes */
    buf = 0;
    for (u64 tmp = ptr; buf < size;) {
        m_free(bs_pop_bucket((void *)(page_size * tmp)));
        u64 val = bs_get_page_size(tmp) / page_size;
        buf += val;
        tmp += val;
    }

    return 1;
}

void bs_merge (u64 offset) {
    u64 size = bs_get_page_size(offset) / page_size;
    u64 ptr = 0;

    /* right part */
    if (offset % (size * 2))
        ptr = offset - size;
    /* left part */
    else
        ptr = offset + size;

    if (!bs_scann_table(ptr, size))
        return;
    m_free(bs_pop_bucket((void *)(page_size * offset)));

    if (offset % (size * 2))
        ptr = offset - size;
    else
        ptr = offset;
    size *= 2;

    while (1) {
        u64 tmp;
        if (ptr % (size * 2))
            tmp = ptr - size;
        else
            tmp = ptr + size;

        if (tmp > memory_size / page_size)
            break;

        if (bs_get_val(tmp) & bs_used_bit)
            break;
        if (!(bs_get_val(tmp) & bs_size_bit)) {
            uart_send("Error: page table is wrong\r\n");
            break;
        }

        if (!bs_scann_table(tmp, size))
            break;

        if (ptr % (size * 2))
            tmp = ptr - size;
        else
            tmp = ptr;
        size *= 2;
    }
    if (ptr % size)
        ptr -= size / 2;
    bs_table_set(ptr * page_size, (ptr + size) * page_size, 0);
    log("buddy system", "merge", ptr * page_size, (ptr + size) * page_size);
}

void bs_free (void *addr) {
    /* look up bs_table */
    u64 offset = (u64)addr / page_size;

    log("buddy system", "free", (u64)addr, (u64)addr + bs_get_page_size(offset));
    bs_table_set((u64)addr, (u64)addr + bs_get_page_size(offset), 0);
    bs_merge(offset);
    return ;
}


/* dynamic allocator */
#define da_init_size 0x10000
malloc_struct *dynamic_allocator_ptr = NULL;
void *dynamic_malloc (u64 size) {
    size = 16 + aligned16(size);

    malloc_struct *mm = search_freed_bins(size);
    if (mm) {
        log("dynamic allocator", "allocate", (u64)mm,
                (u64)mm + (mm->info & m_size_mask));
        return (void *)&mm->data;
    }

    if ((dynamic_allocator_ptr->info & m_size_mask) > size) {
    }
    else {
        void *tmp = NULL;
        if (size < da_init_size)
            tmp = bs_malloc(da_init_size);
        else {
            u64 counter;
            u64 s = size;
            for (counter = 0; s; s /= 2) counter++;
            s = 1 << (counter + 2);
            tmp = bs_malloc(s);
        }

        if (!tmp)
            return NULL;
        /* append new memory behind old memory */
        else if ((dynamic_allocator_ptr->info & m_size_mask) + (u64)dynamic_allocator_ptr
                == (u64)tmp) {
            dynamic_allocator_ptr->info += da_init_size;
        }
        else {
            dynamic_allocator_ptr->info |= (m_used_bit | m_last_chunk_bit);
            m_free((void *)dynamic_allocator_ptr->data);

            dynamic_allocator_ptr = (malloc_struct *)tmp;
            dynamic_allocator_ptr->info = da_init_size | m_last_chunk_bit;
        }
    }

    mm = dynamic_allocator_ptr;
    dynamic_allocator_ptr = (malloc_struct *)((u64)mm + size);
    dynamic_allocator_ptr->prev = 0;
    dynamic_allocator_ptr->info = (mm->info - size) | m_last_chunk_bit;
    mm->info = size | m_used_bit;
    log("dynamic allocator", "allocate", (u64)mm,
            (u64)mm + (mm->info & m_size_mask));
    return (void *)mm->data;
}

void dynamic_free(void *addr) {
    malloc_struct *whole_chunk = (malloc_struct *)((u64)addr - 0x10);
    u64 size = whole_chunk->info & 0xfffffffffffffff0;
    whole_chunk->info &= (~m_used_bit);    /* clear used bit */
    /* small bins */
    if (size <= small_bin_size * 0x10 + 0x10 && size >= 0x20) {
        small_bin_list *ptr = (small_bin_list *)&whole_chunk->data;
        u64 index = (size - 0x20) / 0x10;
        ptr->next = small_bin[index].next;
        small_bin[index].next = ptr;
    }
    /* large bins */
    else {
        large_bin_list *ptr = (large_bin_list *)&whole_chunk->data;
        ptr->next = large_bin.next;
        ptr->prev = &large_bin;
        large_bin.next->prev = ptr;
        large_bin.next = ptr;
    }

    /* insert information into the next block */
    if (!(whole_chunk->info & m_last_chunk_bit)) {
        malloc_struct *nextb = (malloc_struct *)((u64)whole_chunk + size);
        nextb->prev = (u64)whole_chunk;
    }

    /* demo information */
    log("dynamid allocator", "free", (u64)whole_chunk, (u64)whole_chunk + size);
}

void dynamic_allocator_init () {
    dynamic_allocator_ptr = (malloc_struct *)bs_malloc(da_init_size);
    if (!dynamic_allocator_ptr) {
        uart_send("Error: dynamic allocator initialization failed\r\n");
        return;
    }
    m_malloc = dynamic_malloc;
    m_free = dynamic_free;
    dynamic_allocator_ptr->prev = 0;
    dynamic_allocator_ptr->info = da_init_size;
}


void bs_malloc_interface (char *buffer) {
    bs_malloc(atoi(&buffer[10]));
}

void bs_free_interface (char *buffer) {
    bs_free((void *)atoui(&buffer[8]));
}

void m_malloc_interface (char *buffer) {
    m_malloc(atoi(&buffer[9]));
}

void m_free_interface (char *buffer) {
    m_free((void *)atoui(&buffer[7]));
}
