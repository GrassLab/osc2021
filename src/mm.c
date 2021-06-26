#include "mm.h"
#include "list.h"
#include "uart.h"
#include "utils.h"
#include "mmu.h"
#include "type.h"

#define MEM_START       (KERNEL_MAPPING+0x10000000)
#define MEM_END         (KERNEL_MAPPING+0x20000000)
#define MAX_ORDER       8
#define MAX_POOL_NUM    30
#define MAX_POOL_PAGE   10
#define MIN_OBJ_SIZE    16
#define CHUNK_BUFF_SIZE 1000

#define get_page_index(entry) \
((unsigned int)(entry - &page[0]))
#define get_address(entry) \
(MEM_START + get_page_index(entry) * PAGE_SIZE)
#define get_chunk_index(entry) \
((unsigned int)(entry - &chunk[0]))

typedef struct {
    unsigned int num;
    list_head_t  head;
} buddy_t;

typedef struct {
    char           used;
    unsigned short order;
    list_head_t    list;
} page_t;

typedef struct {
    char         used;
    unsigned int obj_size;
    unsigned int obj_used[MAX_POOL_PAGE];
    unsigned int page_used;
    page_t       *page[MAX_POOL_PAGE];
    list_head_t  head;
} pool_t;

typedef struct {
    void        *addr;
    list_head_t list;
} chunk_t;

const unsigned int page_num = (MEM_END - MEM_START) / PAGE_SIZE;
buddy_t free_list[MAX_ORDER+1];
page_t  page[(MEM_END - MEM_START) / PAGE_SIZE];
pool_t  pool[MAX_POOL_NUM];
chunk_t chunk[CHUNK_BUFF_SIZE];
unsigned int chunk_pos;

static void print_block(page_t *block) {
    print("Block --> ");
    print("Start index:");
    print_int(get_page_index(block));
    print(", Size: ");
    print_int(pow(2, block->order));
    print(", Used: ");
    if (!block->used) {
        print("No\n");
    } else {
        print("Yes\n");
    }
}

void buddy_print() {
    print("---------------Buddy system-----------------\n\n\n");
    for (int i = 0; i <= MAX_ORDER; i++) {
        print("Order: ");
        print_int(i);
        print(" (");
        print_int(pow(2, i));
        print("), Block num: ");
        print_int(free_list[i].num);
        print("\n");
        if (free_list[i].num < 20 && free_list[i].num) {
            list_head_t *entry = free_list[i].head.next;
            print("=============================================\n");
            while (entry != &free_list[i].head) {
                page_t *block = list_entry(entry, page_t, list);
                print_block(block);
                entry = entry->next;
            }
            print("=============================================\n\n");
        } else {
            print("\n");
        }
    }
    print("------------------end---------------------\n\n\n");
}

/*
 *  Page frame allocator
 */
static void page_allocator_init() {
    /* Initial buddy system */
    for (int i = 0; i <= MAX_ORDER; i++) {
        free_list[i].num = 0;
        list_init(&free_list[i].head);
    }
    /* Initial memory (page) */
    for (int i = 0; i < page_num; i++) {
        page[i].used = 0;
        page[i].order = -1;
        list_init(&page[i].list);
        *((char*)(MEM_START + i)) = 0;
    }
    /* Hook page to buddy system */
    unsigned int p_num   = page_num;
    unsigned int p_count = 0;
    for (int i = MAX_ORDER; i >= 0; i--) {
        unsigned int sum = pow(2,i);
        unsigned int remainder = p_num % sum;
        free_list[i].num = p_num / sum;
        for (int j = 0; j < free_list[i].num; j++) {
            page[p_count].order = i;
            list_add_tail(&free_list[i].head, &page[p_count].list);
            p_count += sum;
        }
        if (!remainder) {
            break;
        } else {
            p_num = remainder;
        }
    }
}

static void buddy_push_block(list_head_t *new,
                             unsigned int order) {
    free_list[order].num++;
    list_add_tail(&free_list[order].head, new);
}

static page_t* buddy_pop_block(list_head_t *entry,
                               unsigned int order) {
    free_list[order].num--;
    list_del(entry);
    return list_entry(entry, page_t, list);
}

static bool_t buddy_release_redundant(unsigned int order) {
    unsigned int higher_order = order + 1;
    while (higher_order <= MAX_ORDER && !free_list[higher_order].num) {
        higher_order++;
    }
    /* No memory */
    if (higher_order > MAX_ORDER)
        return false;
    //print("****************Release redundant***************\n");
    for (int i = higher_order; i > order; i--) {
        page_t *block = buddy_pop_block(free_list[i].head.next, i);
        unsigned int index = get_page_index(block) + pow(2, i - 1);
        block->order--;
        page[index].order = i-1;
        buddy_push_block(&block->list, i - 1);
        buddy_push_block(&page[index].list, i - 1);
        //print_block(&page[index]);
    }
    //print("************************************************\n\n");
    return true;
}

static void buddy_merge(page_t *block) {
    unsigned int block_head = get_page_index(block);
    unsigned int order = block->order;
    unsigned int buddy;
    //print("*********************Find buddy********************\n");
    for (; order < MAX_ORDER; order++) {
        buddy = block_head ^ (1 << order);
        /* Search buddy */
        if (order != page[buddy].order || page[buddy].used != (char)0)
            break;
        /*print("Origin ");
        print_block(&page[block_head]);
        print("Buddy ");
        print_block(&page[buddy]);
        print("-----------------------Merge-----------------------\n");*/
        buddy_pop_block(&page[buddy].list, page[buddy].order);
        if (block_head < buddy) {
            page[buddy].order = -1;
        } else {
            page[block_head].order = -1;
            block_head = buddy;
        }
        page[block_head].order++;
    }
    //print_block(&page[block_head]);
    //print("***************************************************\n\n");
    buddy_push_block(&page[block_head].list, page[block_head].order);
}

static page_t* buddy_alloc(unsigned int order) {
    if (!free_list[order].num)
        if (buddy_release_redundant(order) == false) {
            print("No memory available\n");
            return NULL;
        }

    page_t* alloc_block = buddy_pop_block(free_list[order].head.next, order);
    alloc_block->used = 1;
    /*print("****************Allocate pages***************\n");
    print_block(alloc_block);
    print("*********************************************\n\n");*/
    return alloc_block;
}

static void buddy_free(page_t *block) {
    if (block->used) {
        block->used = 0;
        memset((void*)get_address(block), pow(2, block->order), 0);
        /*print("******************Free pages*****************\n");
        print_block(block);
        print("*********************************************\n\n");*/
        buddy_merge(block);
    } else {
        print("Invalid address 1\n");
    }
}

/*
 *  Object allocator
 */
static void chunk_init() {
    chunk_pos = 0;
    for (int i = 0; i < CHUNK_BUFF_SIZE; i++) {
        chunk[i].addr = NULL;
        list_init(&chunk[i].list);
    }
}

static void pool_init() {
    for (int i = 0; i < MAX_POOL_NUM; i++) {
        pool[i].used = 0;
        list_init(&pool[i].head);
    }
}

static void pool_create(unsigned int size, unsigned int entry) {
    pool_t *p = &pool[entry];
    p->used = 1;
    p->obj_size = size;
    p->page_used = 1;
    p->obj_used[p->page_used - 1] = 0;
    p->page[p->page_used - 1] = buddy_alloc(0);
}

static void* obj_alloc(unsigned int entry) {
    /*print("Pool entry: ");
    print_int(entry);
    print("\n");*/
    pool_t *p = &pool[entry];
    if (p->head.next != &p->head) {
        //print("Get chunk from:  chunk buffer\n\n");
        chunk_t *get_chunk = list_entry(p->head.next, chunk_t, list);
        list_del(p->head.next);
        void *addr = get_chunk->addr;
        get_chunk->addr = NULL;
        if (get_chunk_index(get_chunk) < chunk_pos)
            chunk_pos = get_chunk_index(get_chunk);
        return addr;
    }

    //print("Get chunk from:  page\n\n");
    p->obj_used[p->page_used - 1]++;
    if ((p->obj_size * (p->obj_used[p->page_used - 1] - 1)) >= PAGE_SIZE) {
        if ((p->page[p->page_used] = buddy_alloc(0)) == NULL)
            return NULL;
        p->obj_used[p->page_used] = 1;
        p->page_used++;
    }
    return (void*)(get_address(p->page[p->page_used - 1]) + p->obj_size * (p->obj_used[p->page_used - 1] - 1));
}

static int obj_free(void *addr, unsigned int entry, unsigned int index) {
    pool_t *p = &pool[entry];
    uint64_t start_addr = MEM_START + index * PAGE_SIZE;
    if (!(((uint64_t)addr - start_addr) % p->obj_size) &&
           (uint64_t)addr <= (start_addr + p->obj_size * (p->obj_used[index] - 1))) {
        list_head_t *pos = p->head.next;
        while (pos != &p->head) {
            chunk_t *get_chunk = list_entry(pos, chunk_t, list);
            if (addr == get_chunk->addr) {
                return -1;
            }
            pos = pos->next;
        }

        //print("Put chunk into chunk buffer\n");
        memset(addr, p->obj_size, 0);
        chunk[chunk_pos].addr = addr;
        list_add_tail(&p->head, &chunk[chunk_pos].list);
        while (chunk[++chunk_pos].addr != NULL) {}
    } else {
        return -1;
    }
    return 0;
}

void* kmalloc(unsigned int size){
    if (size > (PAGE_SIZE / 2)) {
        //print("Use page allocator directly\n");
        int order = 0;
        if (!(size % PAGE_SIZE))
            order--;
        size /= (PAGE_SIZE);
        while (size) {
            size /= 2;
            order++;
        }
        return (void*)get_address(buddy_alloc(order));
    }
    else {
        unsigned int fit_size = (size / MIN_OBJ_SIZE) * MIN_OBJ_SIZE + MIN_OBJ_SIZE;
        if (!(size % MIN_OBJ_SIZE))
            fit_size -= MIN_OBJ_SIZE;
        for (int i = 0; i < MAX_POOL_NUM; i++) {
            if (pool[i].used && pool[i].obj_size == fit_size) {
                return obj_alloc(i);
            } else if (!pool[i].used){
                pool_create(fit_size, i);
                return obj_alloc(i);
            }
        }
        print("No available pool\n");
        return NULL;
    }
}

void kfree(void *addr) {
    if (addr < MEM_START || addr >= MEM_END) {
        print("Invalid address 4\n");
        return ;
    }
    unsigned int index = ((uint64_t)addr - (uint64_t)MEM_START) / PAGE_SIZE;
    for (int i = 0; i < MAX_POOL_NUM; i++) {
        if (pool[i].used) {
            for (int j = 0; j < pool[i].page_used; j++) {
                if (index == get_page_index(pool[i].page[j])) {
                    if (obj_free(addr, i, index) < 0)
                        print("Invalid address 2\n");
                    return ;
                }
            }
        } else {
            break;
        }
    }
    if ((((uint64_t)addr - (uint64_t)MEM_START) % PAGE_SIZE)!= 0) {
        print("Invalid address 3\n");
    } else {
        buddy_free(&page[index]);
    }
}

void mm_init() {
    page_allocator_init();
    pool_init();
    chunk_init();
}
