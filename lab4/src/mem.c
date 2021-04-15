# include "mem.h"
# include "buddy.h"
# include "linklist.h"
# include "linklist.c"
# include "uart.h"
# include "my_math.h"

struct mem_node mem_ll[MEM_TABLE_MAX_SIZE];
struct mem_node *mem_inuse;  //already define in mem.h
struct mem_node *mem_unuse;

struct mem_node* get_mem_inuse(){
  return mem_inuse;
}

void mem_init(){
  mem_ll[0].idx = 0;
  mem_ll[0].addr = 0;
  mem_ll[0].bytes = 0;
  mem_ll[0].page_need = 0;
  mem_ll[0].pre = 0;
  mem_ll[0].next = &mem_ll[1];
  for (int i=1; i<MEM_TABLE_MAX_SIZE-1; i++){
    mem_ll[i].idx = i;
    mem_ll[i].addr = 0;
    mem_ll[i].bytes = 0;
    mem_ll[i].page_need = 0;
    mem_ll[i].pre = &mem_ll[i-1];
    mem_ll[i].next = &mem_ll[i+1];
  }
  mem_ll[MEM_TABLE_MAX_SIZE-1].addr = MEM_TABLE_MAX_SIZE-1;
  mem_ll[MEM_TABLE_MAX_SIZE-1].addr = 0;
  mem_ll[MEM_TABLE_MAX_SIZE-1].bytes = 0;
  mem_ll[MEM_TABLE_MAX_SIZE-1].page_need = 0;
  mem_ll[MEM_TABLE_MAX_SIZE-1].pre = &mem_ll[MEM_TABLE_MAX_SIZE-2];
  mem_ll[MEM_TABLE_MAX_SIZE-1].next = 0;
  mem_inuse = 0;
  mem_unuse = &mem_ll[0];
}

void mem_ll_show(){
  uart_puts((char *) "Memory Status\n");
  uart_puts((char *) "address          bytes  pages\n");
  uart_puts((char *) "===============================\n");
  struct mem_node *ite = mem_inuse;
  while(ite){
    char ct[30];
    int_to_hex(ite->addr, ct);
    uart_puts(ct);
    uart_puts((char *) " \t ");
    int_to_str(ite->bytes, ct);
    uart_puts(ct);
    uart_puts((char *) " \t ");
    int_to_str(ite->page_need, ct);
    uart_puts(ct);
    uart_puts((char *) "\n");
    ite = ite->next;
  }
}

void mem_add_new_node(int mbytes, int page_need, unsigned long long r){
  struct mem_node *new_node = ll_pop_front<struct mem_node>(&mem_unuse);
  new_node->addr = r;
  new_node->bytes = mbytes;
  new_node->page_need = page_need;
  //find insert point
  if (!mem_inuse){
    ll_push_front(&mem_inuse, new_node);
    return ;
  }
  struct mem_node *target = mem_inuse;
  while(target->next && target->next->addr < new_node->addr){
    target = target->next;
  }
  if (target->addr > new_node->addr) target = 0;
  ll_push_elm(&mem_inuse, new_node, target);
}

void* malloc(int mbytes){
  void *r = 0;
  int page_need;
  if (mbytes == 0){
    return r;
  }
  if (mbytes > BUDDY_PAGE_SIZE/2){
    char ct[20];
    //cal page need
    page_need = mbytes/BUDDY_PAGE_SIZE;
    page_need = (mbytes%BUDDY_PAGE_SIZE) ? page_need+1 : page_need ;
    int_to_str(mbytes, ct);
    uart_puts((char *) "Alloc memory size ");
    uart_puts(ct);
    uart_puts((char *) ", need page ");
    int_to_str(page_need, ct);
    uart_puts(ct);
    uart_puts((char *) ", at order ");
    int page_need_o = page_need-1;
    //cal order
    int order = 0;
    while(page_need_o){
      page_need_o >>= 1;
      order++;
    }
    int_to_str(order, ct);
    uart_puts(ct);
    uart_puts((char *) "\n");
    //call buddy
    r = (void *)buddy_alloc(mbytes, order, 0);
  }
  else{
    r = (void *)buddy_dma_alloc(mbytes, 0);
    page_need = -1;
  }
  //add r in mem table
  if(r) mem_add_new_node(mbytes, page_need, (unsigned long long)r);
  return r;
}

void free(void* addr){
  struct mem_node *target = mem_inuse;
  while(target && (target->addr != (unsigned long long)addr )){
    target = target->next;
  }
  if(target){
    if (target->bytes > BUDDY_PAGE_SIZE/2)
      buddy_free(target->addr, target->bytes, 0);
    else
      buddy_dma_free(target->addr, target->bytes, 0);
    ll_rm_elm<struct mem_node>(&mem_inuse, target);
    target->addr = 0;
    target->bytes = 0;
    target->page_need = 0;
    ll_push_front<struct mem_node>(&mem_unuse, target);
  }
  else{
    uart_puts((char *) "Memory addr <");
    char ct[20];
    int_to_hex((unsigned long long)addr, ct);
    uart_puts(ct);
    uart_puts((char *) "> not found\n");
  }
}
