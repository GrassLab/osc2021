#include "uart.h"
#include "utils.h"
#include "page_malloc.h"
#include "stdlib.h"
#define page_base 8
#define minimum_base 4
#define minimum_order 16
#define frame_list_amount 18

extern unsigned long __heap_start; /* declared in the linker script */
static Page frame_freelist[frame_list_amount + 1] = {{0},{NULL}};
Page address_index = {0,NULL};
char tmp_string[20];
unsigned long addr_base;

void kb_indicator(int order, int lr){
    int full_order = (order + minimum_base >= 10)? order + minimum_base - 10 : order + minimum_base;
    itoa((1UL << full_order), tmp_string);
    //uart_puts(tmp_string);
    //if(order + minimum_base >= 10)
        //uart_puts("KB");
    //else
        //uart_puts("B");
    //if(lr)
        //uart_puts("\n");
}

int pow_2(int order){
    int n = 1;
    for(int i=order; i>0; i--)
        n*=2;
    return n;
}

int order_2(unsigned long address){
    int n = 0;
    while(!(address & 1) && n < (frame_list_amount + minimum_base)){
        address = address >> 1;
        n++;
    }
    return n;
}

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
    //uart_puts("remove 0x");
    dec_hex(n->next->addr,tmp_string);
    //uart_puts(tmp_string);
    //uart_puts(" from frame index: ");
    kb_indicator(order,1);

    Page* temp = n->next;
    n->next = n->next->next;
    free(temp);
}

void insert_framelist(int order, unsigned int addr){
    insert_Page(&frame_freelist[order], createNewPage(addr));

    dec_hex(addr,tmp_string);
    //uart_puts("insert 0x");
    //uart_puts(tmp_string);
    //uart_puts(" to frame index: ");
    kb_indicator(order,1);
}

void alloc_page_init(unsigned long addr_low, unsigned long addr_high){
    if(addr_low % minimum_order != 0)
        addr_low = addr_low / minimum_order * minimum_order + 16;
    else
        addr_low = addr_low / minimum_order * minimum_order;
    
    unsigned long mem_size = addr_high - addr_low;
    unsigned long addr_low_pre = addr_low;
    addr_base = addr_low;
    int order;
    //uart_puts("-----------------------------------------------------------");
    //uart_puts("------------\n");

    Page* page_index_order = &address_index;
    insert_Page(page_index_order, createNewPage(addr_low));
    page_index_order = page_index_order -> next;

    order = order_2(addr_low) - minimum_base;
    while(order>=0){
        if(mem_size >= (1UL << order << minimum_base)){
            insert_framelist(order, addr_low);
            addr_low_pre = addr_low;
            addr_low += (1UL << order << minimum_base);
            mem_size = addr_high - addr_low;
            //uart_puts("initialized mempry spece left: ");
            dec_hex(mem_size,tmp_string);
            //uart_puts(tmp_string);
            //uart_puts("\n");
            if(order_2(addr_low_pre) != order_2(addr_low)){
                order =  order_2(addr_low) - minimum_base ;
                insert_Page(page_index_order, createNewPage(addr_low));
                page_index_order = page_index_order -> next;
            }
                
        }
        else{
            order--;
        }
    }
 
    
    //uart_puts("page init finish\n");   
    //uart_puts("-----------------------------------------------------------");
    //uart_puts("------------\n");
}

Page* find_page(Page* page, int addr){
    Page* previous;
    previous = page;
    while(page != NULL){
        if(page->addr == addr)
            return previous;
        else {
            previous = page;
            page = page->next;
        }
    }
    return NULL;
}

unsigned long find_index_base(int addr){
    Page* page;
    Page *previous;
    page = &address_index;
    previous = page;
    while(page != NULL && addr > page ->addr){
            previous = page;
            page = page->next;
    }
    return previous->addr;
}

int size_to_order(int size){
    int order = 0;
    for(int i = minimum_order; ; i*=2){
        if(size <= i)
            return order;
        order ++;
    }
}

unsigned int alloc_page(int size){
    Page* current;
    int pow2_of_order;
    int order = size_to_order(size);
    int i = order;

    for(; i<=frame_list_amount; i++){
        current = &frame_freelist[i];
        if(current->next != NULL)
            break;
    }
    int remove_index = i;
    if(remove_index > frame_list_amount){
        //uart_puts("there are no enough memory space to fetch\n");
        //uart_puts("-----------------------------------------------------------");
        //uart_puts("------------\n");  
        return 0;
    }
        

    int addr = current->next->addr;
    int divide = (remove_index > order)? 1 : 0;

    //uart_puts("allocated ");
    kb_indicator(order,0);
    //uart_puts(" sapce at: ");
    dec_hex(addr,tmp_string);
    //uart_puts(tmp_string);
    //uart_puts("\n");

    if(remove_index > order){
        //uart_puts("fetch memory space form larger frame index due to memory insufficient\n");
    }
    remove_Page(current, remove_index); 
    while(i >= order){
        i--;
        pow2_of_order = 1UL<<(i + minimum_base);
        if(i > order || (i == order && divide))
            insert_framelist(i, addr + pow2_of_order);
    }
    
    //uart_puts("-----------------------------------------------------------");
    //uart_puts("------------\n");       
    return addr;
}

void free_page(int addr, int size){
    Page* current;
    Page* temp;
    unsigned long index_base = find_index_base(addr);
    unsigned int index = ((addr - index_base) / minimum_order);
    int order = size_to_order(size);
    int i;
    int buddy_addr;
    int buddy_index;
    //uart_puts("release ");
    kb_indicator(order,0);
    //uart_puts(" space at: ");
    dec_hex(addr,tmp_string);
    //uart_puts(tmp_string);
    //uart_puts("\n");

    for(i = order; i<frame_list_amount; i++){
        current = &frame_freelist[i];
        buddy_index = index^pow_2(i);
        buddy_addr = buddy_index * minimum_order + index_base;
        temp = find_page(current, buddy_addr);
        if(temp != NULL){
            remove_Page(temp, i);
            index = index / pow_2(i+1) * pow_2(i+1);
            //asm volatile("nop");
            //uart_puts("merge page from:");
            dec_hex(index*minimum_order + index_base,tmp_string);
            //uart_puts(tmp_string);
            //uart_puts(" ");
            buddy_index = index^pow_2(i);
            buddy_addr = buddy_index * minimum_order + index_base;
            dec_hex(buddy_addr,tmp_string);
            //uart_puts(tmp_string);
            //uart_puts(" to frame index: ");
            kb_indicator(i+1,1);
        }
        else
            break;    
    }
    insert_framelist(i, index*minimum_order + index_base);
    //uart_puts("-----------------------------------------------------------");
    //uart_puts("------------\n");
}
