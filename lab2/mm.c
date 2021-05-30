#include "include/mini_uart.h"
#include "include/cutils.h"
#include "include/csched.h"
#include "include/mm.h"
#include "utils.h"
#define MAX_NR_REGIONS_POOL 20
#define MAX_ORDER 14 // original: 7 15
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1)) // 16384 pages
#define PAGE_SIZE 0x1000 // 4KB
#define PAGE_OFFSET(ADDR) ((PAGE_SIZE - 1) & (ADDR)) 
#define MAX_PD_TYPE 8
#define MAX_DMPP 64

struct region_descriptor rd_pool[MAX_NR_REGIONS_POOL];
struct region_descriptor *free_region_head;
struct region_descriptor *used_region_head;

struct page free_list[MAX_ORDER];
struct page page_frames[MAX_ORDER_NR_PAGES];
struct page_descriptor pd_list[MAX_PD_TYPE];
struct page *dynamic_mem_page_pool[MAX_DMPP];

extern struct task *current;

int insert_into_pdlist(struct page_descriptor *pd_new, struct page_descriptor *pd_prev)
{
    pd_prev->next = pd_new;
    pd_new->prev = pd_prev;
    return 0;
}

int remove_from_pdlist(struct page_descriptor *pd)
{
    pd->prev->next = pd->next;
    if (pd->next)
        pd->next->prev = pd->prev;
    pd->next = 0;
    pd->prev = 0;
    // pd->frame = 0;
    return 0;
}


/* Find first 0-bit in pd->bitmap.
 * return bit index for success, -1 for fail
 */
int get_free_index(struct page_descriptor *pd_walk)
{
    char *bitmap_start;

    bitmap_start = pd_walk->bitmap_start;
    for (int bit = 0; bit < pd_walk->chunk_num; bit++) {
        if (!bit_test(bitmap_start, bit)) {
            return bit;
        }
    }
    return -1;
}

/* chunk_num shall be multiple of 8.
 * return 0 for succes,
 * return 1 for fail. (caller should change chunk_size or chunk_num)
 */
int set_pd(struct page_descriptor *pd,
    struct page *frame, int chunk_size, int chunk_num)
{ 
    int bitmap_len = chunk_num >> 3;
    char *chunk_start = (char*)pd +
                        sizeof(struct page_descriptor) +
                        bitmap_len;
    chunk_start = align_upper(chunk_start, sizeof(chunk_size));
    pd->next = 0;
    pd->prev = 0;
    pd->frame = frame;
    pd->bitmap_start = (char*)pd + sizeof(struct page_descriptor);
    pd->bitmap_len = bitmap_len;
    for (int i = 0; i < bitmap_len; ++i)
        pd->bitmap_start[i] = 0;
    pd->chunk_start = chunk_start;
    pd->chunk_size = chunk_size;
    pd->chunk_num = chunk_num;
    if ((char*)pd + PAGE_SIZE < chunk_start + chunk_size * chunk_num)
        return 1;
    return 0;

}

// return 0 for success, -1 for fail
int pd_list_grow(struct page_descriptor *pd, int chunk_size, int chunk_num)
{
    struct page *frame;
    struct page_descriptor *new_pd;

    if (!(frame = get_free_frames(1)))
        return -1;

    new_pd = (struct page_descriptor*)(frame->addr);
    if (set_pd(new_pd, frame, chunk_size, chunk_num))
        return -1;
    insert_into_pdlist(new_pd, pd);
    return 0;
}

char *kmalloc(int size)
{
    struct page_descriptor *pd_walk;
    int nr_pages, chunk_idx;
    struct page *ret_page;
    char *ret_addr;

    pd_walk = 0;
    for (int i = 0; i < MAX_PD_TYPE; ++i) {
        if (size <= pd_list[i].chunk_size) {
            pd_walk = &pd_list[i];
            break;
        }
    }
    /* If all chunk type can't serve, then
     * we can alloc frame directly for it.
     */
    if (!pd_walk) {
        nr_pages = size % PAGE_SIZE ? size / PAGE_SIZE + 1 : size / PAGE_SIZE;
        if (!(ret_page = get_free_frames(nr_pages)))
            return 0; // get_free_frames fail
        // Record pages borrowed to caller 
        for (int i = 0; i < MAX_DMPP; ++i) {
            if (!dynamic_mem_page_pool[i]) {
                dynamic_mem_page_pool[i] = ret_page;
                break;
            }
        }
        // TODO
        return (char*)(ret_page->addr);
    }
    /* If it comes to here, meaning that pd_walk
     * is a acceptable descriptor.
     */

    while (1) {
        if (!pd_walk->next)
            if (pd_list_grow(pd_walk, pd_walk->chunk_size, pd_walk->chunk_num))
                return 0; // System frames are not enough, return NULL
        pd_walk = pd_walk->next;
        if ((chunk_idx = get_free_index(pd_walk)) < 0) {
            continue; // this page has no free chunk to serve.
        }
        break;
    }

    // To here, we can start cope with address sending back to caller.
    bit_set(pd_walk->bitmap_start, chunk_idx);
    ret_addr = pd_walk->chunk_start + pd_walk->chunk_size * chunk_idx;

    return ret_addr;
}

char *page_of_ptr(char *ptr)
{
    return (char*)(((unsigned long)ptr) & ~(PAGE_SIZE - 1));
}

int get_chunk_index(struct page_descriptor *pd, char *ptr)
{
    return (ptr - pd->chunk_start) / pd->chunk_size;
}

// return 1 for True, 0 for False
int bitmap_is_all_zero(struct page_descriptor *pd)
{
    int bitmap_len;

    bitmap_len = pd->bitmap_len;
    for (int i = 0; i < bitmap_len; ++i)
        if (pd->bitmap_start[i] != 0)
            return 0;
    return 1;
}

// return 0 for normal object, 1 for whole page, -1 for ptr is not legal
int kfree(char* ptr)
{
    struct page_descriptor *pd;
    int chunk_idx;

    if (!PAGE_OFFSET((unsigned long)ptr)) {
        // ptr is a frame pointer, so we can release is directly.
        for (int i = 0; i < MAX_DMPP; ++i) {
            if (dynamic_mem_page_pool[i]->addr == (unsigned long)ptr) {
                free_frames(dynamic_mem_page_pool[i]);
                dynamic_mem_page_pool[i] = 0; // free pool entry
                return 1;
            }
        }
        // If comes to here, meaning that ptr is
        // not what we've hold. It's an illegal
        // address.
        return -1;
    }
    pd = (struct page_descriptor*)page_of_ptr(ptr); // Find the page pointer owning ptr
    chunk_idx = get_chunk_index(pd, ptr);
    bit_clr(pd->bitmap_start, chunk_idx);
    // If bitmap is all zero, then free the page back to buddy system.
    if (bitmap_is_all_zero(pd)) {
        remove_from_pdlist(pd);
        free_frames(pd->frame); // TODO
    }

    return 0;
}

int insert_into_list(struct page *frame)
{
    struct page *head = &free_list[frame->blk_odr];
    frame->next = head->next;
    frame->prev = head;
    head->next = frame;
    if (frame->next)
        frame->next->prev = frame;
    return 0;
}

int remove_from_list(struct page *frame)
{
    frame->prev->next = frame->next;
    if (frame->next)
        frame->next->prev = frame->prev;
    frame->next = 0;
    frame->prev = 0;
    return 0;
}

int block_set_free(struct page *frame, int set_val)
{ // for frame in block: frame.free = set_val;
    int num = 1 << frame->blk_odr;

    for (int i = 0; i < num; ++i)
        frame[i].free = set_val;
    frame->free = set_val;

    return 0;
}

/* 1. If the block is maximum, then add it into
 *    free link-list and return.
 * 2. Find the buddy.
 * 3. If the buddy is free, then merge with it.
 *    If not, then return.
 */
int free_frames(struct page *frame)
{
    int buddy_idx, legal;
    struct page *page_walk, *buddy;

    legal = 0;
    // Check if the frame is a frame we maintained.
    for (int i = 0; i < MAX_ORDER_NR_PAGES; ++i)
        if (frame == &page_frames[i] && !(frame->free)) {
            legal = 1;
            break;
        }
    if (!legal) { // If not, then free failed.
        uart_send_string("From free_frames: Try to free illegal frame\r\n");
        return 1;
    }
 
    page_walk = frame;
    while (1) {
        // page_walk->free = 1;

        block_set_free(page_walk, 1);
        if (page_walk->blk_odr == MAX_ORDER-1) { // free largest block
            insert_into_list(page_walk);
            return 0;
        }

        buddy_idx = page_walk->index ^ (1 << page_walk->blk_odr);
        buddy = &page_frames[buddy_idx];

        if (buddy->free && (buddy->blk_odr == page_walk->blk_odr)) { // buddy can be coalesced.
            // uart_send_string("[DEMO] From free_frames: merge!\r\n");
            if (buddy_idx > page_walk->index) { // buddy is at right side
                page_walk->blk_odr++;
                buddy->blk_odr = -1;
                remove_from_list(buddy);
            } else { // buddy is at left side
                buddy->blk_odr++;
                page_walk->blk_odr = -1;
                page_walk = buddy;
                remove_from_list(page_walk);
            }
        } else { // buddy can't be coalesced.
            insert_into_list(page_walk);
            return 0;
        }

    }
}

struct page *get_free_frames(int nr)
// nr : number of pages caller wants.
{
    int target_ord;
    struct page *page_walk, *buddy;

    if (nr > MAX_ORDER_NR_PAGES) {
        uart_send_string("Error: Apply too many pages.\r\n");
        return 0;
    }
    target_ord = 32 - lead_zero(nr - 1); // int is 32bit in arm64
    for (int order = target_ord; order < MAX_ORDER; ++order) {
        if ((page_walk = free_list[order].next)) { // link-list is not empty

            remove_from_list(page_walk);
            while (order > target_ord) { 
                // Still need to split the block
                // uart_send_string("[DEMO] From get_free_frames: split!\r\n");
                order--;
                buddy = page_walk + (1 << order);
                buddy->blk_odr = order;
                insert_into_list(buddy);
            }
            page_walk->blk_odr = order;
            block_set_free(page_walk, 0);
            page_walk->ref = 1;
            // page_walk->free = 0;

            return page_walk;
        }
    }
    uart_send_string("Error: No block can serve.\r\n"); 
    return 0;
}

void dynamic_mem_init()
{
    for (int i = 0; i < MAX_DMPP; ++i)
        dynamic_mem_page_pool[i] = 0;
    for (int i = 0; i < MAX_PD_TYPE; ++i)
    {
        pd_list[i].next = 0;
        pd_list[i].prev = 0;
        pd_list[i].bitmap_start = 0;
        pd_list[i].bitmap_len = 0;
        pd_list[i].chunk_start = 0;
        pd_list[i].chunk_size = 1 << i;
        pd_list[i].chunk_num = 8;
    }
}

void buddy_system_init(unsigned long mem_base)
{
    int i;
    // Initialize page_frames
    for (i = 0; i < MAX_ORDER_NR_PAGES; ++i) {
        page_frames[i].addr = mem_base + i * PAGE_SIZE + 0xffff000000000000;
        page_frames[i].next = 0;
        page_frames[i].prev = 0;
        page_frames[i].index = i;
        page_frames[i].free = 1;
        page_frames[i].blk_odr = -1;
    }
    // Initialize free_list
    for (i = 0; i < MAX_ORDER; ++i) {
        free_list[i].addr = 0;
        free_list[i].next = 0;
        free_list[i].prev = 0;
        free_list[i].index = -1;
        free_list[i].free = -1;
        free_list[i].blk_odr = -2; // -2 is a mark of free_list head
    }
    // In beginning, memory is a max ordered block
    page_frames[0].blk_odr = MAX_ORDER - 1;
    page_frames[0].prev = &free_list[MAX_ORDER-1];
    free_list[MAX_ORDER-1].next = &page_frames[0];
}



int insert_into_rdlist(struct region_descriptor *rd_new, struct region_descriptor *rd_prev)
{
    rd_prev->next = rd_new;
    rd_new->prev = rd_prev;
    return 0;
}

int remove_from_rdlist(struct region_descriptor *rd)
{
    rd->prev->next = rd->next;
    if (rd->next)
        rd->next->prev = rd->prev;
    rd->next = 0;
    rd->prev = 0;
    return 0;
}
// region descriptor set
int rd_set(struct region_descriptor *rd, unsigned long addr,
    unsigned long size, int free)
{
    rd->addr = addr;
    rd->size = size;
    rd->free = free;
    return 0;
}

struct region_descriptor *rd_new(struct region_descriptor pool[])
{
    for (int i = 0; i < MAX_NR_REGIONS_POOL; ++i) {
        if (!(pool[i].used)) {
            pool[i].used = 1;
            return &pool[i];
        }
    }
    return 0; // If there is no available descriptor, return NULL.
}

int rd_request(unsigned long addr, unsigned long size)
{
    struct region_descriptor *rd_walk, *rd_tmp;
    unsigned long w_size, w_addr;

    // Traverse all free regions, find the one covering the request.
    for (rd_walk = free_region_head; rd_walk; rd_walk = rd_walk->next) {
        w_addr = rd_walk->addr;
        w_size = rd_walk->size;
        if (addr + size > w_addr + w_size)
            continue;
        if (addr < w_addr)
            return -1;
        // If it comes to here, meaning that rd_walk
        // can cover whole (addr ~ addr+size) region.
        if ((addr != w_addr) && (addr + size != w_addr + w_size))
        { // request region is seating right inside walk.
            rd_set(rd_walk, w_addr, addr - w_addr, 1);
            rd_tmp = rd_new(rd_pool);
            rd_set(rd_tmp, addr + size, w_addr + w_size - (addr + size), 1);
            insert_into_rdlist(rd_tmp, rd_walk);
        }
        else if ((addr != w_addr) && (addr + size == w_addr + w_size))
        { // request region is stick to the right edge.
            rd_set(rd_walk, w_addr, addr - w_addr, 1);
        }
        else if ((addr == w_addr) && (addr + size != w_addr + w_size))
        { // request region is stick to the left edge.
            rd_set(rd_walk, addr + size, w_addr + w_size - (addr + size), 1);
        }
        else
        { // request region is coincidently fit into the walk.
            remove_from_rdlist(rd_walk);
            rd_set(rd_walk, 0, 0, 0);
        }
        return 0;
    }
    return -1;
}

void rd_pool_init()
{
    for (int i = 0; i < MAX_NR_REGIONS_POOL; ++i) {
        rd_set(&rd_pool[i], 0, 0, 0);
        rd_pool[i].used = 0;
    }
}

int rd_init()
{
    rd_pool_init();
    // At the beginning, we assume memory is a whole block.
    free_region_head = rd_new(rd_pool);
    free_region_head->next = 0;
    free_region_head->prev = 0;
    rd_set(free_region_head, 0, 0x40000000, 1);

    // just try
    if ((rd_request(0, 0x1000)) == -1) {
        uart_send_string("Error: mem-region request failed.");
        return -1;
    }
    // kernel
    if ((rd_request(0x80000, 0x10000)) == -1) {
        uart_send_string("Error: mem-region request failed.");
        return -1;
    }
    // initramfs
    if ((rd_request(0x20000000, 0x1000)) == -1) {
        uart_send_string("Error: mem-region request failed.");
        return -1;
    }
    // buddy system
    if ((rd_request(BUDDY_BASE, MAX_ORDER_NR_PAGES * PAGE_SIZE)) == -1) {
        uart_send_string("Error: mem-region request failed.");
        return -1;
    } else {
        buddy_system_init(BUDDY_BASE);
    }
    return 0;
}

int show_sm()
{
    struct region_descriptor *rd_walk;
    for (rd_walk = free_region_head; rd_walk; rd_walk = rd_walk->next) {
        uart_send_string("Address: ");
        uart_send_ulong((unsigned long)rd_walk);
        uart_send_string("\r\n");
        uart_send_string("next: ");
        uart_send_ulong((unsigned long)(rd_walk->next));
        uart_send_string("\r\n");
        uart_send_string("prev: ");
        uart_send_ulong((unsigned long)(rd_walk->prev));
        uart_send_string("\r\n");
        uart_send_string("addr: ");
        uart_send_ulong(rd_walk->addr);
        uart_send_string("\r\n");
        uart_send_string("size: ");
        uart_send_ulong(rd_walk->size);
        uart_send_string("\r\n");
        uart_send_string("used: ");
        uart_send_int(rd_walk->used);
        uart_send_string("\r\n");
        uart_send_string("free: ");
        uart_send_int(rd_walk->free);
        uart_send_string("\r\n");
    }
    return 0;
}

int show_m()
{
    struct page_descriptor *page_walk;

    uart_send_string("pd_list:\r\n");
    for (int i = 0; i < MAX_PD_TYPE; ++i)
    {
    uart_send_string("=============i = ");
            uart_send_int(i);
    uart_send_string(" ===============\r\n");

        page_walk = &pd_list[i];
        while ((page_walk = page_walk->next)) {
    uart_send_string(" ===============\r\n");
    uart_send_string("\r\nnext: ");

            uart_send_ulong((unsigned long)(page_walk->next));
    uart_send_string("\r\nprev: ");
            uart_send_ulong((unsigned long)(page_walk->prev));
    uart_send_string("\r\nframe->index: ");
            uart_send_int(page_walk->frame->index);
    uart_send_string("\r\npage_walk->chunk_num: ");
            uart_send_int(page_walk->chunk_num);
    uart_send_string("\r\npage_walk->bitmap_len: ");
            uart_send_int(page_walk->bitmap_len);
    uart_send_string("\r\n");

        }
    }
    return 0;
}

int show_mm()
{
    int i;
    uart_send_string("free_list:\r\n");
    for (i = 0; i < MAX_ORDER; ++i)
    {
    uart_send_string("=========\r\n");
        uart_send_string("i:");
        uart_send_int(i);
        uart_send_string("\r\n");
        uart_send_string("Address:");
        uart_send_ulong((unsigned long)&free_list[i]);
        uart_send_string("\r\n");
        uart_send_string("next:");
        uart_send_ulong((unsigned long)free_list[i].next);
        uart_send_string("\r\n");
        uart_send_string("prev:");
        uart_send_ulong((unsigned long)free_list[i].prev);
        uart_send_string("\r\n");
        uart_send_string("blk_odr:");
        uart_send_int(free_list[i].blk_odr);
        uart_send_string("\r\n");
        uart_send_string("free:");
        uart_send_int(free_list[i].free);
        uart_send_string("\r\n");
    }
    uart_send_string("===================\r\n");
    uart_send_string("page_frames:\r\n");
    for (i = 0; i < MAX_ORDER_NR_PAGES; ++i)
    {
    uart_send_string("=========\r\n");

        uart_send_string("i:");
        uart_send_int(i);
        uart_send_string("\r\n");
        uart_send_string("Address:");
        uart_send_ulong((unsigned long)&page_frames[i]);
        uart_send_string("\r\n");
        uart_send_string("next:");
        uart_send_ulong((unsigned long)page_frames[i].next);
        uart_send_string("\r\n");
        uart_send_string("prev:");
        uart_send_ulong((unsigned long)page_frames[i].prev);
        uart_send_string("\r\n");
        uart_send_string("blk_odr:");
        uart_send_int(page_frames[i].blk_odr);
        uart_send_string("\r\n");
        uart_send_string("free:");
        uart_send_int(page_frames[i].free);
        uart_send_string("\r\n");
    }
    return 0;
}

unsigned long *kpgd;

int is_present(unsigned long *pt_ent)
{
    return *pt_ent & 0x1;
}

unsigned long *_alloc_page_table()
{
    struct page *page;
    unsigned long *tar;

    if (!(page = get_free_frames(1))) {
        uart_send_string("From alloc_page_table: fail\r\n");
        return 0;
    }
    tar = (unsigned long*)page->addr;
    for (int i = 0; i < 512; ++i)
        tar[i] = 0;

    return tar;
}

unsigned long *alloc_page_table(struct mm_struct *mm)
{
    struct page *page;
    unsigned long *tar;

    if (!(page = get_free_frames(1))) {
        uart_send_string("From alloc_page_table: fail\r\n");
        return 0;
    }
    mm->pages[mm->page_nr++] = page;
    tar = (unsigned long*)page->addr;
    for (int i = 0; i < 512; ++i)
        tar[i] = 0;

    return tar;
}

int kernel_space_map(unsigned long va)
{
    unsigned long *pt_ent, *pd_walk, pg;

    pd_walk = kpgd;
    pt_ent = &pd_walk[(va >> PGD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)_alloc_page_table();
        *pt_ent = KVA_TO_PA(pg) | BOOT_PGD_ATTR;
    }

    // pd_walk = *pt_ent & PD_ENT_ADDR_MASK;
    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PUD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)_alloc_page_table();
        *pt_ent = KVA_TO_PA(pg) | BOOT_PUD_ATTR;
    }

    // pd_walk = *pt_ent & PD_ENT_ADDR_MASK;
    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PMD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        if (va >= 0x30000000)
            *pt_ent = (va & 0xFFFFFFE00000) | BOOT_PMD_DEV_ATTR;
        else
            *pt_ent = (va & 0xFFFFFFE00000) | BOOT_PMD_NOR_ATTR;
            *pt_ent = (va & 0xFFF) | BOOT_PMD_NOR_ATTR;
    }

    return 0;
}

int kernel_space_map_(unsigned long va)
{
    unsigned long *pt_ent, *pd_walk, pg;

    pd_walk = kpgd;
    pt_ent = &pd_walk[(va >> PGD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)_alloc_page_table();
        *pt_ent = KVA_TO_PA(pg) | BOOT_PGD_ATTR;
    }

    // pd_walk = *pt_ent & PD_ENT_ADDR_MASK;
    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PUD_SHIFT) & PD_IDX_MASK];
    //     uart_send_ulong(va);
    // uart_send_string("\r\n");
    // uart_send_ulong((unsigned long)pt_ent);
    // uart_send_string("\r\n");
    if (!is_present(pt_ent)) {
        *pt_ent = (va & 0xFFFFC0000000) | BOOT_PMD_DEV_ATTR;
    }

    return 0;
}

unsigned long *create_kernel_pgd(unsigned long start, unsigned long length)
{ // length unit is byte
    unsigned long addr_walk, addr_end, step_len;

    kpgd = _alloc_page_table();
    addr_walk = start & 0xFFFFFFE00000;
    addr_end = (start + length) & 0xFFFFFFE00000;
    step_len = 1 << 21;
    while (addr_walk <= addr_end) {
        kernel_space_map(addr_walk);
        addr_walk += step_len;
    }

    set_ttbr1(KVA_TO_PA((unsigned long)kpgd));
    set_ttbr0(KVA_TO_PA((unsigned long)kpgd));
    // set_ttbr1(0x7000);
    // _set_ttbr0(0x7000);

    return kpgd;
}


int load_cpio_file(char *page_addr, char *cpio_start,
    unsigned long offset)
{
    char *from;

    if (offset >= 0x1000) {
        from = align_down(cpio_start + offset, 0x1000);
    }
    else {
        from = cpio_start;
    }
    memcpy(page_addr, from, 0x1000);

    return 0;
}

// int user_space_map(unsigned long va, int anon)
// {
//     unsigned long *pt_ent, *pd_walk, pg;
//     char *page;

//     pd_walk = current->mm->pgd;
//     pt_ent = &pd_walk[(va >> PGD_SHIFT) & PD_IDX_MASK];
//     if (!is_present(pt_ent)) {
//         pg = (unsigned long)alloc_page_table();
//         *pt_ent = KVA_TO_PA(pg) | BOOT_PGD_ATTR;
//     }

//     pd_walk = (unsigned long*)pg;
//     pt_ent = &pd_walk[(va >> PUD_SHIFT) & PD_IDX_MASK];
//     if (!is_present(pt_ent)) {
//         pg = (unsigned long)alloc_page_table();
//         *pt_ent = KVA_TO_PA(pg) | BOOT_PUD_ATTR;
//     }

//     pd_walk = (unsigned long*)pg;
//     pt_ent = &pd_walk[(va >> PMD_SHIFT) & PD_IDX_MASK];
//     if (!is_present(pt_ent)) {
//         pg = (unsigned long)alloc_page_table();
//         *pt_ent = KVA_TO_PA(pg) | BOOT_PMD_ATTR;
//     }

//     pd_walk = (unsigned long*)pg;
//     pt_ent = &pd_walk[(va >> PTE_SHIFT) & PD_IDX_MASK];
//     if (!is_present(pt_ent)) {
//         pg = (unsigned long)alloc_page_table();
//         *pt_ent = KVA_TO_PA(pg) | BOOT_PTE_NOR_ATTR;
//         if (!anon){
//             load_cpio_file((char*)pg, current->mm->cpio_start, va);
//         }
//     }
//     return 0;
// }

int load_content_to_page(char *page_kva, unsigned long offset,
    struct mm_struct *mm, struct vm_area_struct *vma)
{
    // TODO: copy amount may be affected by file size.
    memcpy(page_kva, mm->cpio_start + offset, 0x1000);
    return 0;
}

int direct_map(struct mm_struct *mm, unsigned long va, unsigned long p_va)
{
    unsigned long *pt_ent, *pd_walk, pg;
    char *page;

    pd_walk = mm->pgd;
    pt_ent = &pd_walk[(va >> PGD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)alloc_page_table(mm);
        *pt_ent = KVA_TO_PA(pg) | BOOT_PGD_ATTR;
    }

    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PUD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)alloc_page_table(mm);
        *pt_ent = KVA_TO_PA(pg) | BOOT_PUD_ATTR;
    }

    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PMD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent)) {
        pg = (unsigned long)alloc_page_table(mm);
        *pt_ent = KVA_TO_PA(pg) | BOOT_PMD_ATTR;
    }

    pd_walk = (unsigned long*)pg;
    pt_ent = &pd_walk[(va >> PTE_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent))
        *pt_ent = KVA_TO_PA(p_va) | BOOT_PTE_NOR_ATTR;

    return 0;
}


unsigned long *create_user_pgd(struct mm_struct *mm)
{ // length unit is byte
    unsigned long addr_walk, addr_end, step_len, page;
    struct vm_area_struct *vma;

    vma = mm->mmap;
    while (vma) {
        addr_walk = vma->vm_start; // vm_start should align to 0x1000
        addr_end = vma->vm_end;
        step_len = 0x1000;
        while (addr_walk <= addr_end) {
            page = (unsigned long)alloc_page_table(mm); // va
            if (!(vma->vm_flag & VM_ANONYMOUS)) {// Only file-page needs to load content
                load_content_to_page((char*)page, addr_walk, mm, vma);
            }
            direct_map(mm, addr_walk, page);
            addr_walk += step_len;
        }
        vma = vma->vm_next;
    }
    return current->mm->pgd;
}

int destroy_mmap(struct mm_struct *mm)
{ // Destory vma link list
    struct vm_area_struct *vma_walk, *vma_last;

    vma_walk = mm->mmap;
    while (vma_walk) {
        vma_last = vma_walk;
        vma_walk = vma_walk->vm_next;
        /* Free vma_last */
        vma_last->vm_end = 0;
    }
    return 0;
}

int destroy_pgd(struct mm_struct *mm)
{
    // Clear all pages: i starting from 1, because
    // pages[0] is pgd. We only need to zero it.
    for (int i = 1; i < mm->page_nr; ++i)
        if (--(mm->pages[i]->ref)) {// when ref becomes zero, free the page.
            free_frames(mm->pages[i]);
        }
    // Zero pgd
    for (int i = 0; i < 512; ++i)
        mm->pgd[i] = 0;
    return 0;
}

int vma_insert(struct vm_area_struct *head, struct vm_area_struct *new)
{
    struct vm_area_struct *vma_walk;

    
    for (vma_walk = head; vma_walk; vma_walk = vma_walk->vm_next) {
        if (!vma_walk->vm_next) {
            if (new->vm_start > vma_walk->vm_end) {
                vma_walk->vm_next = new;
                return 0;
            }
            return -1;
        } else {
            if (new->vm_end < vma_walk->vm_next->vm_start &&
                new->vm_start > vma_walk->vm_end) {
                vma_walk->vm_next = new;
                new->vm_next = vma_walk->vm_next;
                return 0;
            }
        }
    }
    return -1;
}

int create_mmap(struct mm_struct *mm)
{
    struct vm_area_struct *vma;

    /* .text vma */
    vma = (struct vm_area_struct *)((unsigned long)new_vma() + 0xffff000000000000);
    vma->vm_start = 0x0;
    vma->vm_end = mm->cpio_size;
    vma->vm_prot = 0;
    vma->vm_flag = VM_SHARED;
    mm->mmap = vma;

    /* stack vma */
    vma = (struct vm_area_struct *)((unsigned long)new_vma() + 0xffff000000000000);
    vma->vm_start = align_down(0x0000fffffffffff0, 0x1000);
    vma->vm_end = 0x0000fffffffffff0;
    vma->vm_prot = 0;
    vma->vm_flag = VM_ANONYMOUS;
    vma_insert(mm->mmap, vma);

    return 0;
}

int create_user_space(struct mm_struct *mm)
{
    int from_kernel;

    from_kernel = 1;
    if (mm->mmap) { // Happens when user program calls exec()
        from_kernel = 0;
        destroy_mmap(mm);
        destroy_pgd(mm);
    }
    create_mmap(mm);
    /* create_user_pgd() was used when
       we don't have demand paging. */
    create_user_pgd(mm);
    if (1 || from_kernel){
        set_ttbr0(KVA_TO_PA((unsigned long)(mm->pgd)));
    }

    return 0;
}

unsigned long va_to_page(unsigned long va, struct mm_struct *mm)
{ // return virtual address of page of va
    unsigned long *pt_ent, *pd_walk;

    pd_walk = mm->pgd;
    pt_ent = &pd_walk[(va >> PGD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent))
        return 0;
    
    pd_walk = PA_TO_KVA(*pt_ent & PD_ENT_ADDR_MASK);
    pt_ent = &pd_walk[(va >> PUD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent))
        return 0;

    pd_walk = PA_TO_KVA(*pt_ent & PD_ENT_ADDR_MASK);
    pt_ent = &pd_walk[(va >> PMD_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent))
        return 0;

    pd_walk = PA_TO_KVA(*pt_ent & PD_ENT_ADDR_MASK);
    pt_ent = &pd_walk[(va >> PTE_SHIFT) & PD_IDX_MASK];
    if (!is_present(pt_ent))
        return 0;

    return PA_TO_KVA(*pt_ent & PD_ENT_ADDR_MASK);
}



int copy_user_space(struct mm_struct *mm_dest, struct mm_struct *mm_src)
{
    struct vm_area_struct *vma_src, *vma_dest;
    unsigned long addr_walk, addr_end, step_len, page_dest, page_src;
    struct page *frame;

    /* Copy vma and page table */
    mm_dest->mmap = (struct vm_area_struct *)((unsigned long)new_vma() + 0xffff000000000000);
    vma_dest = mm_dest->mmap;
    for (vma_src = mm_src->mmap; vma_src; ) {
        // Copy vma
        memcpy((char*)vma_dest, (char*)vma_src, sizeof(struct vm_area_struct));
        // Copy page table
        addr_walk = vma_dest->vm_start; // vm_start should align to 0x1000
        addr_end = vma_dest->vm_end;
        step_len = 0x1000;
        while (addr_walk <= addr_end) {
            page_src = va_to_page(addr_walk, mm_src); // Parent's page
            if (vma_dest->vm_flag & VM_SHARED) { // shared pages
                frame = (struct page*)VA_TO_FRAME(page_src);
                frame->ref++;
                mm_dest->page_nr++;
                direct_map(mm_dest, addr_walk, page_src);
            }
            else { // private page needs its own page
                page_dest = (unsigned long)alloc_page_table(mm_dest); // va
                memcpy((char*)page_dest, (char*)page_src, 0x1000);
                direct_map(mm_dest, addr_walk, page_dest);
            }
            addr_walk += step_len;
        }
        // Iterate vma_src
        if ((vma_src = vma_src->vm_next)) {
            vma_dest->vm_next = (struct vm_area_struct *)((unsigned long)new_vma() + 0xffff000000000000);
            vma_dest = vma_dest->vm_next;
        }
    }
    /* Copy others */
    mm_dest->cpio_start = mm_src->cpio_start;
    mm_dest->cpio_size = mm_src->cpio_size;

    return 0;
}


struct vm_area_struct *addr_to_vma(unsigned long addr, struct mm_struct *mm)
{
    struct vm_area_struct *vma;

    vma = mm->mmap;
    while (vma) {
        if ((addr >= vma->vm_start) && (addr <=vma->vm_end))
            return vma;
        vma = vma->vm_next;
    }
    return 0; // NULL
}

#define DFSC_TF_0 0x4               // 000100 "Translation fault, level 0" PGD fault
#define DFSC_TF_1 0x5               // 000101 "Translation fault, level 1" PUD fault
#define DFSC_TF_2 0x6               // 000110 "Translation fault, level 2" PMD fault
#define DFSC_TF_3 0x7               // 000111 "Translation fault, level 3" PTE fault
#define DFSC_PERMISSION_FAULT_1 0xD // 001101 "Permission fault, level 1"
/* Data Fault Status Code
 * DFSC: ESR_ELx[5:0]
 * DDI0487C_a_armv8_arm.pdf p.2463
 */
int do_mem_abort(unsigned long addr, unsigned long esr)
{
    unsigned long dfsc;
    unsigned long page;
    struct mm_struct *mm;
    struct vm_area_struct *vma;

    dfsc = (esr & 0x3F);
    uart_send_string("From do_mem_abort: A\r\n");
    uart_send_ulong(dfsc);
    uart_send_string("\r\n");
    if ((dfsc & 0b111100) == 0b100) { // range from 4 ~ 7: Translation fault
        mm = current->mm;
        if (!(vma = addr_to_vma(addr, mm))) { // addr is not in address space
            uart_send_string("Segmentation fault: failing at ");
            uart_send_ulong(addr);
            uart_send_string("\r\n");
            sys_exit();
        }
        // addr is in address space, so we have to prepare the page for it.
        // TODO: check vma->vm_prot is legal
        page = (unsigned long)alloc_page_table(mm); // va
        if (!(vma->vm_flag & VM_ANONYMOUS)) // Only file-page needs to load content
            load_content_to_page((char*)page, addr & 0xFFFFFFFFFFFFF000, mm, vma);
        direct_map(mm, addr, page);
        return 0;
    }
    uart_send_string("Error: failed by other reason\r\n");
    sys_exit();
}
