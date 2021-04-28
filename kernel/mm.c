#include "../include/mm.h"
#include "../include/uart.h"
#include "../include/string.h"


void memory_init(){

    // Initial memory page
    for(int i=0;i<MAX_ORDER_SIZE;i++){
        pagelist[i].pg_num = i;
        pagelist[i].order = -1;
        pagelist[i].used = 0;
        pagelist[i].addr =  START_MEMORY + i*PAGE_SIZE;
        pagelist[i].cache_idx = -1;
        pagelist[i].cache_num = 0;
        INIT_list_head_t(&pagelist[i].list);
    }

    // Initial freelist
    for(int i=0;i<=MAX_ORDER;i++){
        INIT_list_head_t(&freelist[i]);
    }

    // Add page[0] to freelist[7]
    list_add_tail(&pagelist[0].list, &freelist[MAX_ORDER]);

    // Initial object_pool list
    for(int i=0;i<=MAX_POOL_ORDER;i++){
        object_pool[i].cur_page = NULL;
        object_pool[i].freehead = NULL;
        object_pool[i].cache_size = 16*pow(2,i);
    }

    memory_dump();
}

void memory_dump(){

    uart_puts("Freelist : \r\n");
    for(int i=0; i<=MAX_ORDER; i++){
        uart_puts("\torder ");
        uart_hex(i);
        list_head_t *next;
        for(next = freelist[i].next; next!=&freelist[i]; next = next->next){
            uart_puts(" -> page[");
            uart_hex(((struct page *)next)->pg_num);
            uart_puts("]");
        }
        uart_puts("\r\n");
    }
    uart_puts("\r\n");
    
}

struct page *page_alloc(int order){

    uart_puts("\r\n========order");
    uart_hex(order);
    uart_puts("========\r\n");
    uart_puts("<Before page malloc>\r\n");
    memory_dump();
    
    int find_ord = 0;
    struct page* alloc;
    uart_puts("\r\n");
    uart_puts("<After page malloc>\r\n");
    for(int i=order;i<=MAX_ORDER;i++){
        // find sutible page size
        if(list_empty(&freelist[i])){
            // uart_hex(i);
            // uart_puts("=");
            continue;
        }
        // remove page from freelist
        alloc = freelist[i].next;
        list_remove(&alloc->list, &alloc->list);
        alloc->used = 1;
        alloc->order = order;
        find_ord = i;
        break;
    }

    // iterative add page to freelist
    while(--find_ord >= order){
        int buddy_pg = FIND_BUDDY_PG(alloc->pg_num, find_ord);
        struct page* buddy = &pagelist[buddy_pg];
        buddy->order = find_ord;
        list_add_tail(&buddy->list, &freelist[find_ord]);
        memory_dump();
    } 
    memory_dump();
    uart_puts("Allocated order : ");
    uart_hex(alloc->order);
    uart_puts("\t");
    uart_puts("page num : ");
    uart_hex(alloc->pg_num);
    uart_puts("\t");
    uart_puts("page addr : ");
    uart_hex(alloc->addr);
    uart_puts("\t");
    uart_puts("\r\n");

    return alloc;
}

void page_free(struct page* free_p){

    uart_puts("\r\n================\r\n");
    uart_puts("Free order : ");
    uart_hex(free_p->order);
    uart_puts("\tpage num : ");
    uart_hex(free_p->pg_num);
    uart_puts("\tpage addr : ");
    uart_hex(free_p->addr);
    uart_puts("\r\n<Before page free>\r\n");
    memory_dump();

    uart_puts("<After page free>\r\n");
    free_p->used = 0;
    int buddy_pg = FIND_BUDDY_PG(free_p->pg_num, free_p->order);
    struct page *buddy = &pagelist[buddy_pg];

    // merge free page
    while(buddy->order == free_p->order && buddy->used == 0){
        list_remove(&buddy->list, &buddy->list);
        struct page *left, *right;
        if(free_p->pg_num < buddy->pg_num){
            left = free_p;
            right = buddy;
        }
        else{
            left = buddy;
            right = free_p;
        }
        left->order += 1;
        right->order = -1;

        free_p = left;
        buddy_pg = FIND_BUDDY_PG(free_p->pg_num, free_p->order);
        buddy = &pagelist[buddy_pg];
        memory_dump();
    }

    list_add_tail(&free_p->list, &freelist[free_p->order]);
    memory_dump();

}

int suitable_obj_size(int size){

    int obj_size = 0;
    if(size<=16)    obj_size = 16;
    else if(size<=32)   obj_size = 32;
    else if(size<=64)   obj_size = 64;
    else if(size<=128)  obj_size = 128;
    else if(size<=256)  obj_size = 256;
    else    obj_size = 512;

    return obj_size;
    
}

void init_object_alloc(unsigned long page_addr, int obj_size){

    for(int i=0;i<PAGE_SIZE/obj_size;i++){
        unsigned long *addr = (unsigned long *)(page_addr+(i*obj_size));
        // uart_hex(addr);
        // uart_puts("\t");
        *addr = page_addr+((i+1)*obj_size);
        // uart_hex(*addr);
        // uart_puts("\n");
    }
    unsigned long *t2 = (unsigned long *)(page_addr + PAGE_SIZE - obj_size);
    *t2 = (unsigned long)NULL;
}

unsigned long *kmalloc(int size){

    int obj_size = suitable_obj_size(size);
    int cache_idx = exp(obj_size/16);
    uart_puts("====Start kmalloc====\r\n");
    uart_puts("<Before kmalloc>\r\n");
    uart_puts("object_pool[cache_idx].freehead: ");
    uart_hex(object_pool[cache_idx].freehead);
    uart_puts("\n");

    uart_puts("\r\n");
    uart_puts("<After kmalloc>\r\n");

    // object_pool list is null
    if(object_pool[cache_idx].freehead == NULL){
        // require new page and init page object
        struct page *cur_page = page_alloc(0);
        unsigned long addr = cur_page->addr;
        init_object_alloc(addr, obj_size);
        cur_page->cache_idx = cache_idx;
        cur_page->cache_num++;
        object_pool[cache_idx].freehead = (unsigned long*)(addr+obj_size);
        object_pool[cache_idx].cur_page = cur_page;
        object_pool[cache_idx].cache_size = obj_size;

        uart_puts("Pool is null\n");
        uart_puts("object page addr : ");
        uart_hex(cur_page->addr);
        uart_puts("\n");
        uart_puts("object page : ");
        uart_hex(cur_page->pg_num);
        uart_puts("\n");
        uart_puts("object_pool[cache_idx].freehead : ");
        uart_hex(object_pool[cache_idx].freehead);
        uart_puts("\n");
    
        return (unsigned long*)addr;

    }
    else{

        unsigned long addr = (unsigned long)object_pool[cache_idx].freehead;
        int pg_num = ceil((addr - START_MEMORY) / PAGE_SIZE);
        struct page *cur_page = &pagelist[pg_num];
        object_pool[cache_idx].freehead = (unsigned long *)*object_pool[cache_idx].freehead;
        cur_page->cache_num++;
        uart_puts("Pool is not null\n");
        uart_puts("object page addr : ");
        uart_hex(object_pool[cache_idx].cur_page->addr);
        uart_puts("\n");
        uart_puts("object page : ");
        uart_hex(object_pool[cache_idx].cur_page->pg_num);
        uart_puts("\n");
        uart_puts("object_pool[cache_idx].freehead: ");
        uart_hex(object_pool[cache_idx].freehead);
        uart_puts("\n");

        return (unsigned long*)addr;
    }
}

void kfree(unsigned long addr){

    uart_puts("====Start kfree====\r\n");
    int pg_num = ceil((addr - START_MEMORY) / PAGE_SIZE);
    struct page *cur_page = &pagelist[pg_num];
    int cache_idx = cur_page->cache_idx;

    unsigned long tmp = object_pool[cache_idx].freehead;
    unsigned long *adr = addr;
    *adr = addr;
    adr = (unsigned long *)addr;
    *adr = tmp;

    cur_page->cache_num--;
    if(cur_page->cache_num == 0){
        object_pool[cache_idx].freehead = NULL;
        cur_page->cache_idx = -1;
        page_free(cur_page);
    }

    memory_dump();
}