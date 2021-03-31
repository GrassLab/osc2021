#include "stdlib.h"
#include "page_malloc.h"

void alloc_print(unsigned long address){
    uart_puts("alloc address at: ");
    uart_int(address, 0x10);
    uart_puts("\n");
}

void free_print(unsigned long address){
    uart_puts("free  address at: ");
    uart_int(address, 0x10);
    uart_puts("\n");
}

void test(){
    Page* page1 = (Page*) malloc(sizeof(Page));
    alloc_print(page1);
    Page* page2 = (Page*) malloc(sizeof(Page));
    alloc_print(page2);
    Page* page3 = (Page*) malloc(sizeof(Page));
    alloc_print(page3);
    Page* page4 = (Page*) malloc(sizeof(Page));
    alloc_print(page4);
    Page* page5 = (Page*) malloc(sizeof(Page));
    alloc_print(page5);

    free(page4);
    free_print(page4);
    free(page3);
    free_print(page3);
    

    page3 = (Page*) malloc(sizeof(Page));
    alloc_print(page3);
    page4 = (Page*) malloc(sizeof(Page));
    alloc_print(page4);
    page5 = (Page*) malloc(sizeof(Page));
    alloc_print(page5);
}