#include "allocator.h"

extern void Infinite_Loop();

extern uint64_t KERNEL_ADDR_END;
extern uint64_t INITRAMFS_ADDR;
extern uint64_t INITRAMFS_ADDR_END;
extern uint64_t DTB_ADDR;

static Buddy_System buddy;
static size_t TOTAL_NUM_OF_PAGE;
static size_t MAX_ORDER;

static size_t log2ceil(unsigned int num){
    int leadingz = __builtin_clz(num);
    int trailingz = __builtin_ctz(num);
    if(leadingz + trailingz == (sizeof(unsigned int) * 8) - 1){
        return trailingz;
    }
    return (sizeof(unsigned int) * 8) - leadingz;
}

static void set_2bitarray(uint16_t *array, size_t index, uint8_t value){
    size_t i = index >> 3;
    size_t rem = index - (i << 3);
    uint16_t mask = 0xC000 >> (rem << 1);
    uint16_t set = ((value & 0x3) << 14) >> (rem << 1);
    array[i] &= ~mask;
    array[i] |= set;
}

static uint8_t read_2bitarray(uint16_t *array, size_t index){
    size_t i = index >> 3;
    size_t rem = index - (i << 3);
    uint16_t value = array[i];
    value >>= 16 - ((rem + 1) << 1);
    return value &= 0x3;
}

void buddy_init(){
    if(sizeof(Page) > PAGE_SIZE){
        miniuart_send_S("Error: struct Page exceed PAGE_SIZE!!!" NEW_LINE);
        Infinite_Loop(1);
    }

    const uint32_t *prop = dtb_get_node_prop_addr("memory", NULL);
    uint32_t value[16];
    uint32_t value_count = 0;
    if(!prop){
        miniuart_send_S("Error: dtb has no 'memory' node!!!" NEW_LINE);
        Infinite_Loop(1);
    }
    while(prop){
        int32_t n = dtb_get_prop_value("reg", prop, &value + value_count);
        if(n == -1){
            miniuart_send_S("Error: 'memory' node has no 'reg' property!!!" NEW_LINE);
            Infinite_Loop(1);
        }
        if(n > 64){
            miniuart_send_S("Error: 'memory->reg' property causes value buffer overflows!!!" NEW_LINE);
            Infinite_Loop(1);
        }
        prop = dtb_get_node_prop_addr("memory", prop);
        value_count += n;
    }
    value_count >>= 2;

    for(size_t i=0; i<value_count; i+=2){
        for(size_t j=0; j<value_count-i-2; j+=2){
            if(value[j] > value[j+2]){
                uint32_t t0 = value[j];
                uint32_t t1 = value[j+1];
                value[j] = value[j+2];
                value[j+1] = value[j+3];
                value[j+2] = t0;
                value[j+3] = t1;
            }
        }
    }

    uint64_t total_size = 0;
    for(size_t i=0; i<value_count; i+=2){
        uint64_t segment = (uint64_t)bswap32(value[i]) + (uint64_t)bswap32(value[i+1]);
        if(segment > total_size){
            total_size = segment;
        }
    }
    total_size = 1 << log2ceil(total_size);
    printf("total size = %lx" NEW_LINE, total_size);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);

    TOTAL_NUM_OF_PAGE = total_size / PAGE_SIZE;
    MAX_ORDER = (__builtin_ctz(TOTAL_NUM_OF_PAGE) + 1);

    buddy.page_frame_bit = (void *)0x10U;
    buddy.free_page_list = (void *)0x10U + (sizeof(uint16_t) * (TOTAL_NUM_OF_PAGE >> 3));
    strfill((void *)buddy.page_frame_bit, 0, sizeof(uint16_t) * (TOTAL_NUM_OF_PAGE >> 3));
    strfill((void *)buddy.free_page_list, 0, sizeof(Page *) * MAX_ORDER);

    size_t res_mem_index_start = (((uint64_t)bswap32(value[value_count-2]) + (uint64_t)bswap32(value[value_count-1])) & ~(0xFFFU)) / PAGE_SIZE;
    size_t res_mem_index_end = TOTAL_NUM_OF_PAGE - 1;
    for(size_t i=res_mem_index_start; i<=res_mem_index_end; i++){
        set_2bitarray(buddy.page_frame_bit, i, 3);
    }
    printf("res mem from %ld to %ld\n", res_mem_index_start, res_mem_index_end);
    for(int64_t i=(int64_t)value_count-4; i>=0; i-=2){
        res_mem_index_start = (((uint64_t)bswap32(value[i]) + (uint64_t)bswap32(value[i+1])) & ~(0xFFFU)) / PAGE_SIZE;
        res_mem_index_end = (((uint64_t)bswap32(value[i+2]) + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE;;
        for(size_t j=res_mem_index_start; j<=res_mem_index_end; j++){
            set_2bitarray(buddy.page_frame_bit, j, 3);
        }
        printf("res mem from %ld to %ld\n", res_mem_index_start, res_mem_index_end);
    }

    size_t kernel_index_end = ((KERNEL_ADDR_END + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE;
    for(size_t i=0; i<=kernel_index_end; i++){
        set_2bitarray(buddy.page_frame_bit, i, 3);
    }
    printf("kernel from %ld to %ld\n", 0, kernel_index_end);

    if(INITRAMFS_ADDR != 0){
        size_t initramfs_index_start = (INITRAMFS_ADDR & ~(0xFFFU)) / PAGE_SIZE;
        size_t initramfs_index_end = ((INITRAMFS_ADDR_END + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE;
        for(size_t i=initramfs_index_start; i<=initramfs_index_end; i++){
            set_2bitarray(buddy.page_frame_bit, i, 3);
        }
        printf("ramfs from %ld to %ld\n", initramfs_index_start, initramfs_index_end);
    }

    fdt_header *blk = (fdt_header *)DTB_ADDR;
    if(bswap32(blk->magic) == DTB_MAGIC_NUM){
        size_t dtb_index_start = (DTB_ADDR & ~(0xFFFU)) / PAGE_SIZE;
        size_t dtb_index_end = ((DTB_ADDR + bswap32(blk->totalsize) + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE;
        for(size_t i=dtb_index_start; i<=dtb_index_end; i++){
            set_2bitarray(buddy.page_frame_bit, i, 3);
        }
        printf("dtb from %ld to %ld\n", dtb_index_start, dtb_index_end);
    }

    for(size_t i=0; i<TOTAL_NUM_OF_PAGE; i++){
        if(read_2bitarray(buddy.page_frame_bit, i) != 3){
            pfree_index(i);
        }
    }

    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
    printf("Buddy System init success at %p" NEW_LINE, BUDDY_START_ADDR);
    printf("%u pages * %u bytes = %u bytes total" NEW_LINE, TOTAL_NUM_OF_PAGE, PAGE_SIZE, TOTAL_NUM_OF_PAGE * PAGE_SIZE);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
}

static void palloc_index(size_t index){
    int32_t order = -1;
    Page *current;
    for(size_t i=MAX_ORDER-1; i>=0; i--){
        if(!buddy.free_page_list[i]){
            continue;
        }
        Page *prev = NULL;
        current = buddy.free_page_list[i];
        while(current){
            if((index >= current->index) && (index <= (current->index + (1 << i) - 1))){
                if(prev){
                    prev->next = current->next;
                }else{
                    buddy.free_page_list[i] = current->next;
                }
                order = i;
                break;
            }
            prev = current;
            current = current->next;
        }
        if(order >= 0){
            break;
        }
        if(i == 0){
            Infinite_Loop(1);
        }
    }

    for(size_t i=order; i>0; i--){
        Page *upper = (Page *)((void *)current + PAGE_SIZE * (1 << (i - 1)));
        upper->index = current->index + (1 << (i - 1));
        if(index >= upper->index){
            push_page_list(current, current->index, &buddy.free_page_list[i-1]);
            set_2bitarray(buddy.page_frame_bit, current->index, 1);
            current = upper;
        }else{
            push_page_list(upper, upper->index, &buddy.free_page_list[i-1]);
            set_2bitarray(buddy.page_frame_bit, upper->index, 1);
        }
    }

    set_2bitarray(buddy.page_frame_bit, current->index, 3);
}

static void pfree_index(size_t index){
    size_t order = 0;
    while(1){
        size_t buddy_index = index ^ (1 << order);
        Page *current = buddy.free_page_list[order];
        Page *prev = NULL;
        while(current){
            if(current->index == buddy_index){
                if(index > current->index){
                    set_2bitarray(buddy.page_frame_bit, index, 0);
                    index = current->index;
                }else{
                    set_2bitarray(buddy.page_frame_bit, current->index, 0);
                }
                if(prev){
                    prev->next = current->next;
                }else{
                    buddy.free_page_list[order] = current->next;
                }
                order++;
                break;
            }
            if(current->index > buddy_index){
                current = NULL;
            }else{
                prev = current;
                current = current->next;
            }
        }
        if(!current){
            break;
        }
    }

    set_2bitarray(buddy.page_frame_bit, index, 1);
    push_page_list((void *)BUDDY_START_ADDR + (index * PAGE_SIZE), index, &buddy.free_page_list[order]);
}

void *palloc(size_t num_of_page){
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
    printf("trying to get %lu page(s)" NEW_LINE, num_of_page);

    size_t i, order = log2ceil(num_of_page);
    if(!num_of_page){
        miniuart_send_nC('-', 22);
        miniuart_send_S(NEW_LINE);
        return NULL;
    }
    for(i=order; i<MAX_ORDER; i++){
        if(buddy.free_page_list[i]){
            break;
        }
    }
    if(i == MAX_ORDER){
        miniuart_send_S("Warning: No available page" NEW_LINE);
        return NULL;
    }
    //split
    for(size_t j=i; j>order; j--){
        printf("spliting page[%lu] with order %lu" NEW_LINE, buddy.free_page_list[j]->index, j);
        void *p = buddy.free_page_list[j];
        size_t index = buddy.free_page_list[j]->index;
        buddy.free_page_list[j] = buddy.free_page_list[j]->next;
        printf("push split page into free_page_list (page[%lu] with order %lu)" NEW_LINE, index, j-1);
        set_2bitarray(buddy.page_frame_bit, index, 1);  //lower
        push_page_list(p, index, &buddy.free_page_list[j-1]);
        p += PAGE_SIZE << (j-1);
        index += (1 << (j-1));
        printf("push split page into free_page_list (page[%lu] with order %lu)" NEW_LINE, index, j-1);
        set_2bitarray(buddy.page_frame_bit, index, 1);  //upper
        push_page_list(p, index, &buddy.free_page_list[j-1]);
    }

    printf("allocated page[%lu] with order %lu" NEW_LINE, buddy.free_page_list[order]->index, order);
    set_2bitarray(buddy.page_frame_bit, buddy.free_page_list[order]->index, 2);
    set_2bitarray(buddy.page_frame_bit, buddy.free_page_list[order]->index + (1 << order) - 1, 3);
    void *p = buddy.free_page_list[order];
    buddy.free_page_list[order] = buddy.free_page_list[order]->next;

    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
    return p;
}

void pfree(void *p){
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
    printf("release %p" NEW_LINE, p);

    if(!p){
        miniuart_send_nC('-', 22);
        miniuart_send_S(NEW_LINE);
        return;
    }
    size_t index = (size_t)(p - BUDDY_START_ADDR) / PAGE_SIZE;
    size_t order = 0;
    while(read_2bitarray(buddy.page_frame_bit, index + (1 << order) - 1) != 3){
        order++;
    }
    set_2bitarray(buddy.page_frame_bit, index + (1 << order) - 1, 0);

    printf("it is page[%ld] with order %ld" NEW_LINE, index, order);

    //find buddy to merge
    while(1){
        size_t buddy_index = index ^ (1 << order);
        Page *current = buddy.free_page_list[order];
        Page *prev = NULL;
        while(current){
            if(current->index == buddy_index){
                printf("found its buddy (page[%ld])" NEW_LINE, current->index);
                if(index > current->index){
                    set_2bitarray(buddy.page_frame_bit, index, 0);
                    p = (Page *)(BUDDY_START_ADDR + (current->index * PAGE_SIZE));
                    index = current->index;
                }else{
                    set_2bitarray(buddy.page_frame_bit, current->index, 0);
                }
                if(prev){
                    prev->next = current->next;
                }else{
                    buddy.free_page_list[order] = current->next;
                }
                order++;
                printf("now it is page[%ld] with order %ld" NEW_LINE, index, order);
                break;
            }
            if(current->index > buddy_index){
                current = NULL;
            }else{
                prev = current;
                current = current->next;
            }
        }
        if(!current){
            break;
        }
    }

    printf("push it into free_page_list" NEW_LINE);
    set_2bitarray(buddy.page_frame_bit, index, 1);
    push_page_list(p, index, &buddy.free_page_list[order]);

    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
}

static void push_page_list(void *p, size_t index, Page **list){
    Page *current = *list;
    Page *prev = NULL;
    while(current && ((void *)current < p)){
        prev = current;
        current = current->next;
    }
    if(!prev){
        *list = p;
        (*list)->index = index;
        (*list)->next = current;
        return;
    }
    prev->next = p;
    prev->next->index = index;
    prev->next->next = current;
}

void show_page_frame_bit(){
    for(size_t i=0; i<(TOTAL_NUM_OF_PAGE>>3); i++){
        uint16_t num = buddy.page_frame_bit[i];
        for(size_t j=0; j<8; j++){
            printf("%d", (num & 0xC000) >> 14);
            num = num << 2;
        }
        miniuart_send_S("  ");
        if((i & 7) == 7){
            miniuart_send_S(NEW_LINE);
        }
    }
}

void show_free_page_list(){
    for(size_t i=0; i<MAX_ORDER; i++){
        Page *p = buddy.free_page_list[i];
        printf("[%d] ", i);
        while(p){
            printf("%p (%lu)   ", p, p->index);
            p = p->next;
        }
        miniuart_send_S(NEW_LINE);
    }
}

void *malloc(size_t num_of_byte){
    if(!num_of_byte){
        return NULL;
    }
    if(num_of_byte > sizeof(((Dynamic_Mem_Page *)0)->data)){
        miniuart_send_S("Warning: Exceed malloc size, please get a page instead" NEW_LINE);
        return NULL;
    }
    //minimum 16 bytes per chunk
    uint32_t nchunk = ROUNDUP_MUL16(num_of_byte) >> 4;

    if(!buddy.malloc_page){
        buddy.malloc_page = palloc(1);
        buddy.malloc_page->next = NULL;
        strfill((void *)buddy.malloc_page->state, 0, sizeof(buddy.malloc_page->state));
    }
    Dynamic_Mem_Page *page_list = buddy.malloc_page;
    int16_t *state = page_list->state;

    size_t start_index;
    while(1){
        size_t accumulate = 0;
        uint32_t trap = 0;
        for(size_t i=0; i<251; i++){
            uint8_t status = read_2bitarray(state, i);
            if(trap){
                if(status == 2){
                    trap = 0;
                }
            }else{
                if(status == 2){
                    accumulate = 0;
                }else if(status == 1){
                    trap = 1;
                    accumulate = 0;
                }else{
                    if(!accumulate){
                        start_index = i;
                    }
                    accumulate++;
                }
            }
            if(accumulate == nchunk){
                break;
            }
        }
        if(accumulate == nchunk){
            break;
        }
        if(page_list->next){
            page_list = page_list->next;
            state = page_list->state;
        }else{
            Dynamic_Mem_Page *p = palloc(1);
            p->next = NULL;
            strfill((void *)p->state, 0, sizeof(p->state));
            page_list->next = p;
            page_list = p;
            state = page_list->state;
        }
    }

    set_2bitarray(state, start_index, 1);
    set_2bitarray(state, start_index + nchunk - 1, 2);

    return page_list->data[start_index];
}

void mfree(void *p){
    if(!p){
        return;
    }

    Dynamic_Mem_Page *page = (void *)(((size_t)p & ~(0xFFFU)) - BUDDY_START_ADDR);
    Dynamic_Mem_Page *prev = NULL, *page_list = buddy.malloc_page;
    while(page_list != page){
        if(!page_list){
            Infinite_Loop(2);
        }
        prev = page_list;
        page_list = page_list->next;
    }

    size_t index = ((size_t)p - (size_t)(page_list->data)) / 16;
    int16_t *state = page_list->state;
    for(size_t i=index; i<251; i++){
        if(read_2bitarray(state, i) == 3){
            set_2bitarray(state, i, 0);
            break;
        }
        set_2bitarray(state, i, 0);
    }

    uint32_t is_empty_page = 1;
    for(size_t i=0; i<32; i++){
        if(state[i] != 0){
            is_empty_page = 0;
            break;
        }
    }

    if(is_empty_page){
        if(prev){
            prev->next = page_list->next;
        }else{
            buddy.malloc_page = page_list->next;
        }
        pfree(page_list);
    }
}
