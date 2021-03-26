#include "uart.h"
#include "utils.h"
#include "page_malloc.h"
#include "stdlib.h"
extern unsigned long __heap_start; /* declared in the linker script */
static Page frame_freelist[11] = {0,NULL};
char tmp_string[20];
unsigned long addr_base;

Page* createNewPage(int addr) {
    Page* temp = (Page*) malloc(sizeof(Page));
    temp->addr = addr;
    temp->next = NULL;
    return temp;
}

void insert_Page(Page* Page1, Page* Page2)
{
    Page2->next = Page1->next;
    Page1->next = Page2;
}

void remove_Page(Page* n, int order)
{
    uart_puts("remove 0x");
    dec_hex(n->next->addr,tmp_string);
    uart_puts(tmp_string);
    uart_puts(" from frame index: ");
    dec_hex(order,tmp_string);
    uart_puts(tmp_string);
    uart_puts("\n");

    Page* temp = n->next;
    n->next = n->next->next;
    free(temp);
}

void insert_framelist(int order, unsigned int addr){
    insert_Page(&frame_freelist[order], createNewPage(addr));

    dec_hex(addr,tmp_string);
    uart_puts("insert 0x");
    uart_puts(tmp_string);
    uart_puts(" in frame index: ");
    dec_hex(order,tmp_string);
    uart_puts(tmp_string);
    uart_puts("\n");
}

void alloc_page_init(unsigned long addr_low, unsigned long addr_high){
    unsigned long mem_size = addr_high - addr_low;
    addr_base = addr_low;
    int i = 10;
    uart_puts("-----------------------------------------------------------");
    uart_puts("------------\n");
    while(i>=0){
        if(mem_size >= (1UL<<i)*4096){
            insert_framelist(i,addr_low);
            addr_low += (1UL<<i)*4096;
            mem_size = addr_high - addr_low;
            uart_puts("initialized mempry spece left: ");
            dec_hex(mem_size,tmp_string);
            uart_puts(tmp_string);
            uart_puts("\n");
        }
        else{
            i--;
        }
    }
    uart_puts("page init finish\n");   
    uart_puts("-----------------------------------------------------------");
    uart_puts("------------\n");
}

int find_page(Page* page, int addr){
    Page* previous;
    previous = page;
    while(page->next != NULL) {
        if(page->addr == addr){
            page = previous;
            return 1;
        }    
        previous = page;        
        page = page->next;
    } 
    if(page->addr == addr){
        page = previous;
        return 1;
    } 
    return 0;
}

int size_to_order(int size){
    int order = 12;
    while(!(size >> order & 1)) order++;
    order -= 12;
    return order;
}

unsigned int alloc_page(int size){
    Page* current;
    int pow2_of_order;
    int order = size_to_order(size);
    int i = order;

    for(; i<=10; i++){
        current = &frame_freelist[i];
        if(current->next != NULL)
            break;
    }
    int remove_index = i;
    if(remove_index > 10){
        uart_puts("there are no enoufh memory space to fetch\n");
        uart_puts("-----------------------------------------------------------");
        uart_puts("------------\n");  
        return 0;
    }
        

    int addr = current->next->addr;
    int divide = (remove_index > order)? 1 : 0;

    uart_puts("allocated ");
    itoa(1UL<<(order+2),tmp_string);
    uart_puts(tmp_string);
    uart_puts("KB page at: ");
    dec_hex(addr,tmp_string);
    uart_puts(tmp_string);
    uart_puts("\n");

    if(remove_index > order){
        uart_puts("fetch memory space form larger frame index due to memory insufficient\n");
    }
    remove_Page(current, remove_index); 
    while(i >= order){
        i--;
        pow2_of_order = 1UL<<(i + 12);
        if(i > order || (i == order && divide))
            insert_framelist(i, addr + pow2_of_order);
    }
    
    uart_puts("-----------------------------------------------------------");
    uart_puts("------------\n");       
    return addr;
}

void free_page(int addr, int size){
    Page* current, temp;
    unsigned int index = (addr - addr_base) / 4096;
    int order = size_to_order(size);
    int i;
    uart_puts("release ");
    itoa(1UL<<(order+2),tmp_string);
    uart_puts(tmp_string);
    uart_puts("KB page at: ");
    dec_hex(addr,tmp_string);
    uart_puts(tmp_string);
    uart_puts("\n");

    for(i = order; i<10; i++){
        current = &frame_freelist[i];
        if(find_page(current, (index^(1UL<<i))*4096 + addr_base)){
            remove_Page(current, i);
            index = index / (1UL<<(i+1)) * (1UL<<(i+1));
            asm volatile("nop");
            uart_puts("merge page from:");
            dec_hex(index*4096 + addr_base,tmp_string);
            uart_puts(tmp_string);
            uart_puts(" ");
            dec_hex((index^(1UL<<i))*4096 + addr_base,tmp_string);
            uart_puts(tmp_string);
            uart_puts(" to frame index: ");
            dec_hex(i,tmp_string);
            uart_puts(tmp_string);
            uart_puts("\n");
        }
        else
            break;    
    }
    insert_framelist(i, index*4096 + addr_base);
    uart_puts("-----------------------------------------------------------");
    uart_puts("------------\n");
}
