#include "../include/memAlloc.h"
#include "../include/uart.h"

#define PAGE_SIZE 4096
#define MEM_SIZE 0x10000000
#define MEM_START 0x10000000
#define PAGE_NUM (MEM_SIZE/PAGE_SIZE)

static int page_arr[PAGE_NUM];

int mem_init(){

    for(int i = 0; i < PAGE_NUM; ++i){
        page_arr[i] = 0;
    }


    return 0;
}
