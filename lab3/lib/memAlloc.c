#include "../include/memAlloc.h"
#include "../include/uart.h"

#define PAGE_SIZE 4096
#define MEM_SIZE 0x10000000 //0.25G
#define MEM_START 0x10000000
#define PAGE_NUM (MEM_SIZE/PAGE_SIZE)

static node_info page_arr[PAGE_NUM];
static list_node list_node_arr[17];
static list_node pool[65536];
static int used = 0;

void list_node_init(){

    for(int i = 0;i < 17; ++i){
        list_node_arr[i].next = &list_node_arr[i];
        list_node_arr[i].prev = &list_node_arr[i];
    }

    list_node_push(&pool[used], 16);
    pool[used].buddy_index = -1;
    pool[used].start_addr = 0x10000000;
    pool[used].fr_no = 0;
    used++;
}

int check_list(int index){
    if(list_node_arr[index].next == &list_node_arr[index]){
        return false;
    }else{
        int address = list_node_arr[index].next -> start_addr;
        return address;
    }
}

void list_node_pop(int exp){

    //list_node* temp = list_node_arr[exp].next;
    //list_node_arr[exp].next = list_node_arr[exp].next -> next;
    //list_node_arr[exp].prev -> prev = list_node_arr[exp].next -> prev;
    //temp -> next = NULL;
    //temp -> prev = NULL;
    list_node_del(list_node_arr[exp].next);


}

void list_node_push(list_node* entry, int exp){

    entry->next = list_node_arr[exp].next;
    entry->prev = list_node_arr[exp].next -> prev;
    list_node_arr[exp].next -> prev  = entry;
    list_node_arr[exp].next = entry;

}

void list_node_del(list_node* entry){
    //uart_printf("%x\n",entry->next -> start_addr);
    //if(entry->prev == &list_node_arr[15]){
    //    uart_puts("test\n");
    //}
    entry -> prev -> next = entry ->next;
    entry -> next -> prev = entry -> prev;
    entry -> next = NULL;
    entry -> prev = NULL;
}

void mem_status_dump(){

    for(int i = 16 ; i>=0; --i){
        int max_item = 17 - i ;
        int iter = 0;
        uart_printf("size 2^%d \n",i+12);
        list_node* temp = list_node_arr[i].next;
        while(temp != &list_node_arr[i]  && iter < max_item ){
            uart_printf("start at 0x%x ", temp -> start_addr);
            temp = temp->next;
            iter++;
        }
        uart_printf("\n");
    }

}

void split_node(int exp){

    uart_printf("split from size 2^%d to 2^%d\n",exp+12,exp+11);
    int addr1,addr2;
    addr1 = list_node_arr[exp].next -> start_addr;
    addr2 = (addr1 + (1<<(exp+11)) ) ;

    int fr_no1,fr_no2;
    fr_no1 = list_node_arr[exp].next -> fr_no;
    fr_no2 = fr_no1 + 1<<(exp-1);
    //uart_printf("%d\n",fr_no1);
    list_node_pop(exp);
    pool[used].start_addr = addr1;
    pool[used].fr_no = fr_no1;
    pool[used].buddy_index = fr_no2;

    int page_no = (addr1- MEM_START)>> 12;
    int frame_size = 1 <<(exp-1);
    while(frame_size > 0){
        page_arr[page_no].corespond_list_node = &pool[used];
        page_arr[page_no].buddy = fr_no2;
        page_arr[page_no].exp = exp -1;
        page_no++;
        frame_size--;
    }

    list_node_push(&pool[used], exp-1);
    used++;
    pool[used].start_addr = addr2;
    pool[used].fr_no = fr_no2;
    pool[used].buddy_index = fr_no1;

    page_no = (addr2- MEM_START)>> 12;
    frame_size = 1 <<(exp-1);
    while(frame_size > 0){
        page_arr[page_no].corespond_list_node = &pool[used];
        page_arr[page_no].buddy = fr_no1;
        page_arr[page_no].exp = exp -1;
        page_no++;
        frame_size--;
    }
    list_node_push(&pool[used], exp-1);
    used++;
}

int find_space(int exp){

    int address = check_list(exp-12);
    if(address == 0){
        uart_printf("Currently no desired size frame, need to split\n");
        return NULL;
    }else{
        uart_printf("found contiguous space of 2^%d at address:0x%x\n",exp, address);
        return address;
    }

}

void* my_alloc(int size){

    //find the 4KB roundup
    int exp = 12;
    while((1 << exp)<size){
        ++exp;
    }

    find_space(exp);

    int done = 0;
    while(!check_list(exp-12)){
        for(int i = exp-11; i <17; ++i){
            if(check_list(i)){
                split_node(i);
                break;
            }
        }

    }
    int address = find_space(exp);
    int page_no = (address- MEM_START)>> 12;
    int frame_size = 1 <<(exp-12);
    while(frame_size > 0){
        page_arr[page_no].inused = 1;
        page_no++;
        frame_size--;
    }
    list_node_pop(exp-12);
    //if(list_node_arr[15].next -> prev == &list_node_arr[15]) uart_puts("111\n");
    return address;

}

void my_free(void *addr){

    int page_no = ((int)addr-MEM_START) >> 12;
    int exp = page_arr[page_no].exp;
    int frame_size = 1<<exp;
    int buddy = page_arr[page_no].buddy;
    list_node* freed = page_arr[page_no].corespond_list_node;

    for(int i = 0; i < frame_size ; ++i){
        page_arr[page_no].inused = 0;
    }
    list_node_push(freed, exp );
    int free = 1;
    for(int i=0;i<frame_size;++i){
        if(page_arr[buddy++].inused == 1){
            free = 0;
        }
    }


    if(free == 1){
        uart_printf("merge 2 size 2^%d\n",page_arr[page_no].exp+12);
    }
 //   uart_printf("%x\n",page_arr[buddy].corespond_list_node -> start_addr);
    //uart_printf("%x\n",page_arr[0].corespond_list_node-> start_addr);
    list_node_del(freed);
    list_node_del(page_arr[--buddy].corespond_list_node);
    pool[used].fr_no = ;
}

int mem_init(){

    list_node_init();

    for(int i = 0; i < PAGE_NUM; ++i){
        page_arr[i].inused = 0;
        page_arr[i].buddy = -1;
        page_arr[i].exp = -1;
    }
    return 0;
}
