#include "../include/list.h"

#define START_MEMORY                    0x10000000
#define MAX_ORDER                       7
#define MAX_ORDER_SIZE                  (1<<MAX_ORDER)
#define PAGE_SIZE                       4096
#define MAX_POOL_ORDER                  5       // 16*2^5=512bytes
#define FIND_BUDDY_PG(pg, order)       ((pg) ^ (1<<(order)))

struct page{

    struct list_head list; // pointer to prev and next
    int order;
    int pg_num;
    int used;
    unsigned long addr; // store frame memory address
    int cache_idx;
    int cache_num;
};

struct cache{

    struct page* cur_page;
    unsigned long *freehead;
    int cache_size;

};

struct page pagelist[MAX_ORDER_SIZE]; // total 128*4KB;
struct list_head freelist[MAX_ORDER+1]; // freelist 2^0*4KB ~ 2^7*4KB
struct cache object_pool[MAX_POOL_ORDER+1];  // object_pool 2^0*16B ~ 2^5*16B = 16B ~ 512B

int exp(int);
void write_ptr(void *p);
void memory_init();
void memory_dump();
struct page *page_alloc();
void page_free();
unsigned long *kmalloc(int size);
int suitable_obj_size(int size);
void init_object_alloc(unsigned long page_addr, int obj_size);
void kfree(unsigned long addr);

