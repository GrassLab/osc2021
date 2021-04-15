# include "buddy.h"
# include "uart.h"
# include "my_math.h"
# include "linklist.h"
# include "linklist.c"
# include "mem.h"

char buddy_table[BUDDY_TABLE_ROWS][BUDDY_TABLE_COLS*BUDDY_TABLE_OFFSET+BUDDY_TABLE_OFFSET+2];
struct buddy_node buddy_ll[BUDDY_LL_MAX_NUM];
struct buddy_node *buddy_head[BUDDY_MAX_ORDER+1];
struct buddy_node *buddy_unuse_head;

struct dma_node dma_ll[BUDDY_DMA_LL_MAX_SIZE];
struct dma_node *dma_head[BUDDY_DMA_SLOT_NUM];
struct dma_node *dma_unuse;

inline int buddy_addr_to_pn(unsigned long long addr){
  return (int)(addr-BUDDY_BASE_ADDR)/BUDDY_PAGE_SIZE;
}

inline unsigned long long buddy_pn_to_addr(int pn){
  return (unsigned long long)pn*BUDDY_PAGE_SIZE+BUDDY_BASE_ADDR;
}

void buddy_uart_puts(char *c, int it){
  for (int i=0; i<it; i++){
    uart_puts((char *) "  ");
  }
  uart_puts(c);
}

void buddy_dma_init(){
  uart_puts((char *) "");
  dma_ll[0].idx = 0;
  uart_puts((char *) "");
  dma_ll[0].chunk_size = -1;
  uart_puts((char *) "");
  for(int t=0; t<BUDDY_DMA_BITSET_LEN; t++) dma_ll[0].bitset[t] = 0;
  uart_puts((char *) "");
  dma_ll[0].free_chunk_num = -1;
  uart_puts((char *) "");
  dma_ll[0].max_chunk_num = -1;
  uart_puts((char *) "");
  dma_ll[0].addr = 0;
  uart_puts((char *) "");
  dma_ll[0].next = &dma_ll[1];
  uart_puts((char *) "");
  dma_ll[0].pre = 0;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].idx = 0;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].chunk_size = -1;
  uart_puts((char *) "");
  for(int t=0; t<BUDDY_DMA_BITSET_LEN; t++) dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].bitset[t] = 0;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].free_chunk_num = -1;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].max_chunk_num = -1;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].addr = 0;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].next = 0;
  uart_puts((char *) "");
  dma_ll[BUDDY_DMA_LL_MAX_SIZE-1].pre = &dma_ll[BUDDY_DMA_LL_MAX_SIZE-2];
  uart_puts((char *) "");
  for (int i=0; i<BUDDY_DMA_LL_MAX_SIZE; i++){
    dma_ll[i].idx = i;
    dma_ll[i].chunk_size = -1;
    for(int t=0; t<BUDDY_DMA_BITSET_LEN; t++) dma_ll[i].bitset[t] = 0;
    dma_ll[i].free_chunk_num = -1;
    dma_ll[i].max_chunk_num = -1;
    dma_ll[i].addr = 0;
    dma_ll[i].next = &dma_ll[i+1];
    dma_ll[i].pre = &dma_ll[i-1];
  }
  for (int i=0; i<BUDDY_DMA_SLOT_NUM; i++){
    dma_head[i] = 0;
  }
  dma_unuse = &dma_ll[0];
}

void dma_register_node(int slot_no, int itrn){
  buddy_uart_puts((char *) "Register free page from buddy system\n", itrn);
  struct dma_node *new_node = ll_pop_front<struct dma_node>(&dma_unuse);
  new_node->chunk_size = slot_no*BUDDY_DMA_UNIT_SIZE;
  new_node->free_chunk_num = BUDDY_PAGE_SIZE/(new_node->chunk_size);
  new_node->max_chunk_num = BUDDY_PAGE_SIZE/(new_node->chunk_size);
  bitset_clrall(new_node->bitset, BUDDY_DMA_BITSET_LEN);
  unsigned long long addr = buddy_alloc(4095, 0, itrn);
  new_node->addr = addr;
  ll_push_back<struct dma_node>(&dma_head[slot_no], new_node);
  buddy_uart_puts((char *) "return\n", itrn);
}

unsigned long long buddy_dma_alloc(int mbytes, int itrn){
  char ct[20];
  int slot_no = round_up(mbytes, BUDDY_DMA_UNIT_SIZE);
  int chunk_size = align_up(mbytes, BUDDY_DMA_UNIT_SIZE);
  buddy_uart_puts((char *) "Dynamic alloc, assign ", itrn);
  int_to_str(mbytes, ct);
  uart_puts(ct);
  uart_puts((char *) " bytes to slot ");
  int_to_str(slot_no, ct);
  uart_puts(ct);
  uart_puts((char *) " ( chunk size = ");
  int_to_str(chunk_size, ct);
  uart_puts(ct);
  uart_puts((char *) " ).\n");
  struct dma_node *target = dma_head[slot_no];
  while(target && target->free_chunk_num == 0){
    target = target->next;
  }
  if(!target){
    buddy_uart_puts((char *) "Slot ", itrn+1);
    int_to_str(slot_no, ct);
    uart_puts(ct);
    uart_puts((char *) " don't have free chunk.\n");
    dma_register_node(slot_no, itrn+1);
    target = dma_head[slot_no];
    while(target && target->free_chunk_num == 0){
      target = target->next;
    }
  }
  buddy_uart_puts((char *) "Find free chunk, base addr <", itrn+1);
  int_to_hex(target->addr, ct);
  uart_puts(ct);
  uart_puts((char *) ">.\n");
  int chunk_no = bitset_get_first_zero(target->bitset, target->max_chunk_num);
  buddy_uart_puts((char *) "Assign chunk number ", itrn+1);
  int_to_str(chunk_no, ct);
  uart_puts(ct);
  unsigned long long r = 0;
  if (chunk_no >= 0){
    target->free_chunk_num--;
    bitset_set(target->bitset, chunk_no, target->max_chunk_num);
    r = target->addr+chunk_size*chunk_no;
    uart_puts((char *) ", addr <");
    int_to_hex(r, ct);
    uart_puts(ct);
    uart_puts((char *) ">.\n");
  }
  buddy_uart_puts((char *) "return\n", itrn);
  return r;
}

void buddy_dma_free(unsigned long long addr, int mbytes, int itrn){
  char ct[20];
  int slot_no = round_up(mbytes, BUDDY_DMA_UNIT_SIZE);
  int chunk_size = align_up(mbytes, BUDDY_DMA_UNIT_SIZE);
  int chunk_no = (addr%BUDDY_PAGE_SIZE)/chunk_size;
  buddy_uart_puts((char *) "Dynamic memory, free ", itrn);
  int_to_str(mbytes, ct);
  uart_puts(ct);
  uart_puts((char *) " bytes from addr <");
  int_to_hex(addr, ct);
  uart_puts(ct);
  uart_puts((char *) ">, base addr <");
  unsigned long long base_addr = addr-(addr%BUDDY_PAGE_SIZE);
  int_to_hex(base_addr, ct);
  uart_puts(ct);
  uart_puts((char *) ">\n");
  buddy_uart_puts((char *) "Chunk size = ", itrn+1);
  int_to_str(chunk_size, ct);
  uart_puts(ct);
  uart_puts((char *) ", number = ");
  int_to_str(chunk_no, ct);
  uart_puts(ct);
  uart_puts((char *) "\n");
  struct dma_node *target = dma_head[slot_no];
  while(target && target->addr != base_addr){
    target = target->next;
  }
  int slot_bit_check = bitset_get(target->bitset, chunk_no, target->max_chunk_num);
  if(slot_bit_check == 1){
    bitset_clr(target->bitset, chunk_no, target->max_chunk_num);
    target->free_chunk_num++;
    if (target->free_chunk_num == target->max_chunk_num){
      buddy_uart_puts((char *) "This slot is all free, release page <", itrn+1);
      int_to_hex(target->addr, ct);
      uart_puts(ct);
      uart_puts((char *) ">\n");
      buddy_free(target->addr, 4095, itrn+1);
      ll_rm_elm<struct dma_node>(&dma_head[slot_no], target);
      uart_puts((char *) "");
      target->chunk_size = -1;
      uart_puts((char *) "");
      target->free_chunk_num = -1;
      uart_puts((char *) "");
      target->max_chunk_num = -1;
      uart_puts((char *) "");
      target->addr = 0;
      ll_push_front<struct dma_node>(&dma_unuse, target);
    }
  }
  buddy_uart_puts((char *) "return\n", itrn);
}

void buddy_dma_ll_show(){
  char tt[20];
  for (int i=0; i<BUDDY_DMA_SLOT_NUM; i++){
    int_to_str(i, tt);
    uart_puts((char *) "Slot ");
    uart_puts(tt);
    uart_puts((char *) ", chunk size ");
    int_to_str(i*BUDDY_DMA_UNIT_SIZE, tt);
    uart_puts(tt);
    if (dma_head[i]){
      uart_puts((char *) "\n");
      struct dma_node *t = dma_head[i];
      while(t){
        uart_puts((char *) "  ");
        int it = 0;
        int used_chunk = t->max_chunk_num-t->free_chunk_num;
        int_to_str(used_chunk, tt);
        uart_puts(tt);
        uart_puts((char *) "/");
        int_to_str(t->max_chunk_num, tt);
        uart_puts(tt);
        for (int j = used_chunk; j>0;it++){
          if (bitset_get(t->bitset, it, t->max_chunk_num) == 1){
            int_to_hex(t->addr+it*i*BUDDY_DMA_UNIT_SIZE, tt);
            uart_puts((char *) "   ");
            uart_puts(tt);
            j--;
          }
        }
        t = t->next;
        uart_puts((char *) "\n");
      }
    }
    else{
      uart_puts((char *) ": is null\n");
    }
  }
}

void buddy_init(){
  buddy_ll[0].idx = 0;
  buddy_ll[0].order = -1;
  buddy_ll[0].addr = 0;
  buddy_ll[0].pre = 0;
  buddy_ll[0].next = &buddy_ll[1];
  buddy_ll[BUDDY_LL_MAX_NUM-1].idx = BUDDY_LL_MAX_NUM-1;
  buddy_ll[BUDDY_LL_MAX_NUM-1].order = -1;
  buddy_ll[BUDDY_LL_MAX_NUM-1].addr = 0;
  buddy_ll[BUDDY_LL_MAX_NUM-1].pre = &buddy_ll[BUDDY_LL_MAX_NUM-2];
  buddy_ll[BUDDY_LL_MAX_NUM-1].next = 0;
  for (int i=1; i<BUDDY_LL_MAX_NUM-1; i++){
    buddy_ll[i].idx = i;
    buddy_ll[i].order = -1;
    buddy_ll[i].addr = 0;
    buddy_ll[i].pre = &buddy_ll[i-1];
    buddy_ll[i].next = &buddy_ll[i+1];
  }
  for (int i=0; i<BUDDY_MAX_ORDER+1; i++){
    buddy_head[i] = 0;
  }
  buddy_unuse_head = &buddy_ll[0];

  buddy_free(BUDDY_BASE_ADDR, BUDDY_SIZE, 0);
}

void buddy_push_free_page(struct buddy_node **head, struct buddy_node *node, int itrn){
  char ct[20];
  buddy_uart_puts((char *) "Push free page, addr <", itrn);
  int_to_hex(node->addr, ct);
  uart_puts(ct);
  uart_puts((char *) "> into order ");
  int_to_str(node->order, ct);
  uart_puts(ct);
  uart_puts((char *) ".\n");
  struct buddy_node *head_t = *head;
  //empty. just insert
  if (!head_t){
    *head = node;
    node->next = 0;
    node->pre = 0;
    buddy_uart_puts((char *) "return\n", itrn);
    return ;
  }
  //find insert position head_t = 0 -> insert at head
  while(head_t->next && head_t->next->addr < node->addr){
    head_t = head_t->next;
  }
  if (node->addr < head_t->addr){
    head_t = 0;
  }

  int order = node->order;
  if(order < BUDDY_MAX_ORDER){
    //node page idx shift
    int npis = buddy_addr_to_pn(node->addr) >> (order+1);
    //head page idx shift
    int hpis;
    //head next page idx shift
    int hnpis;
    struct buddy_node *del_node = 0;
    //check if coalesce
    if (head_t){
      hpis = buddy_addr_to_pn(head_t->addr) >> (order+1);
      hnpis = (head_t->next) ? buddy_addr_to_pn(head_t->next->addr) >> (order+1) : -1;
      if(npis == hpis){
        del_node = head_t;
        node->addr = head_t->addr;
        node->order ++;
      }
      else if (npis == hnpis){
        del_node = head_t->next;
        node->order ++;
      }
    }
    else{
      hpis = 0;
      hnpis = buddy_addr_to_pn((*head)->addr) >> (order+1);
      if (npis == hnpis){
        del_node = *head;
        node->order ++;
      }
    }
    //del this order's linklist and add to upper order
    if(del_node){
      buddy_uart_puts((char *) "Find addr<", itrn+1);
      int_to_hex(del_node->addr, ct);
      uart_puts(ct);
      uart_puts((char *) ">, <");
      int_to_hex(node->addr, ct);
      uart_puts(ct);
      uart_puts((char *) "> can be coalesced.\n");
      ll_rm_elm<struct buddy_node>(&buddy_head[order], del_node);
      ll_push_front<struct buddy_node>(&buddy_unuse_head, del_node);
      buddy_push_free_page(&buddy_head[order+1], node, itrn+1);
    } //or insert node
    else{
      ll_push_elm<struct buddy_node>(&buddy_head[order], node, head_t);
    }
  } //max order, just insert
  else{
    ll_push_elm<struct buddy_node>(&buddy_head[order], node, head_t);
  }
  buddy_uart_puts((char *) "return\n", itrn);
}

unsigned long long buddy_alloc(int mbytes, int order, int itrn){
  char ct[20];
  buddy_uart_puts((char *) "Find free page in order ", itrn);
  int_to_str(order, ct);
  uart_puts(ct);
  uart_puts((char *) "\n");
  if(!buddy_head[order]){
    buddy_uart_puts((char *) "This order is null\n", itrn+1);
    unsigned long long r;
    if (order >= BUDDY_MAX_ORDER) r = 0;
    else r = buddy_alloc(mbytes, order+1, itrn+1);
    buddy_uart_puts((char *) "return\n", itrn);
    return r;
  }
  buddy_uart_puts((char *) "Find free page <", itrn+1);
  struct buddy_node *alloc_node= ll_pop_front<struct buddy_node>(&buddy_head[order]);
  unsigned long long r = alloc_node->addr;
  int_to_hex(r, ct);
  uart_puts(ct);
  uart_puts((char *) ">\n");

  int page_need = mbytes/BUDDY_PAGE_SIZE;
  page_need = (mbytes%BUDDY_PAGE_SIZE == 0) ? page_need : page_need+1;

  buddy_uart_puts((char *) "Page need = ", itrn+1);
  int_to_str(page_need, ct);
  uart_puts(ct);
  int page_redundant = (1 << order) - page_need;
  uart_puts((char *) ", Redundant page = ");
  int_to_str(page_redundant, ct);
  uart_puts(ct);
  uart_puts((char *) "\n");

  //handle redundant page
  int prnp = buddy_addr_to_pn(r)+page_need; //page redundant, next page
  int prl_order = 0; //page redundant last order
  while(page_redundant){
    if(page_redundant%2){
      struct buddy_node *new_node = ll_pop_front<struct buddy_node>(&buddy_unuse_head);
      new_node->order = prl_order;
      new_node->addr = buddy_pn_to_addr(prnp);
      buddy_push_free_page(&buddy_head[prl_order], new_node, itrn+1);
      prnp += (1 << prl_order);
    }
    page_redundant /= 2;
    prl_order++;
  }
  //
  alloc_node->order = -1;
  ll_push_front(&buddy_unuse_head, alloc_node);
  buddy_uart_puts((char *) "return\n", itrn);
  return r;
}

void buddy_free(unsigned long long base_addr, int mbytes, int itrn){
  char ct[20];
  int page_need = mbytes/BUDDY_PAGE_SIZE;
  page_need =  (mbytes%BUDDY_PAGE_SIZE) ? page_need+1 : page_need;
  buddy_uart_puts((char *) "Free memory, ", itrn);
  int_to_str(page_need, ct);
  uart_puts(ct);
  uart_puts((char *) " page from addr <");
  int_to_hex(base_addr, ct);
  uart_puts(ct);
  uart_puts((char *) ">\n");
  int base_page = buddy_addr_to_pn(base_addr);
  int page_left = page_need;
  int order = 0;
  while(page_left){
    if ((page_left >> order)%2 || order == BUDDY_MAX_ORDER){
      struct buddy_node *new_node = ll_pop_front<struct buddy_node>(&buddy_unuse_head);
      new_node->order = order;
      page_left -= (1 << order);
      new_node->addr = buddy_pn_to_addr(base_page+page_left);
      buddy_push_free_page(&buddy_head[order], new_node, itrn+1);
    }
    if (order < BUDDY_MAX_ORDER) order++;
  }
  buddy_uart_puts((char *) "return\n", itrn);
}

void buddy_ll_show(){
  char tt[20];
  for (int i=0; i<BUDDY_MAX_ORDER+1; i++){
    int_to_str(i, tt);
    uart_puts((char *) "Order ");
    uart_puts(tt);
    uart_puts((char *) ":\n");
    if (buddy_head[i]){
      struct buddy_node *t = buddy_head[i];
      while(t){
        char ttt[20];
        int_to_hex(t->addr, ttt);
        uart_puts((char *) "   ");
        uart_puts(ttt);
        t = t->next;
      }
      uart_puts((char *) "\n");
    }
    else{
      uart_puts((char *) "  null\n");
    }
  }
}

char* buddy_table_get_char_by_pn(int pn){
  int row = pn/BUDDY_TABLE_COLS;
  int col = pn%BUDDY_TABLE_COLS;
  char *r = &buddy_table[row][col*BUDDY_TABLE_TIMES+BUDDY_TABLE_OFFSET+1];
  return r;
}

void buddy_table_put_mem_page(){
  struct mem_node *mem_head = get_mem_inuse();
  while(mem_head){
    int page_num = mem_head->page_need;
    int page_no = buddy_addr_to_pn(mem_head->addr);
    for (int i=0; i<page_num; i++){
      char *tt = buddy_table_get_char_by_pn(page_no++);
      if (i == 0) tt[0] = '<';
      if (i == page_num-1) tt[2] = '>';
      tt[1] = '*';
    }
    mem_head = mem_head->next;
  }
}

void buddy_table_put_free_page(){
  for (int i=0; i<BUDDY_MAX_ORDER+1; i++){
    if (buddy_head[i]){
      struct buddy_node *t = buddy_head[i];
      while(t){
        int page_num = (1 << t->order);
        int page_no = buddy_addr_to_pn(t->addr);
        for (int j=0; j<page_num; j++){
          char *tt = buddy_table_get_char_by_pn(page_no++);
          if (j == 0) tt[0] = '[';
          if (j == page_num-1) tt[2] = ']';
          if (i < 10) tt[1] = (char)(i+48);
          else tt[1] = (char)(i+55);
        }
        t = t->next;
      }
    }
  }
}

void buddy_table_show_init(){
  for (int i=0; i<BUDDY_TABLE_ROWS; i++){
    for (int j=0; j<BUDDY_TABLE_OFFSET+1; j++){
      buddy_table[i][j] = ' ';
    }
    char *t = &buddy_table[i][0];
    int_to_hex_align(i*BUDDY_TABLE_COLS+(BUDDY_BASE_ADDR/BUDDY_PAGE_SIZE), t, BUDDY_TABLE_OFFSET-1);
    for (int j=0; j<BUDDY_TABLE_COLS; j++){
      int tt = j*BUDDY_TABLE_TIMES+BUDDY_TABLE_OFFSET+1;
      buddy_table[i][tt] = ' ';
      buddy_table[i][tt+1] = '-';
      buddy_table[i][tt+2] = ' ';
      if ((j+1)%BUDDY_TABLE_TIMES == 0) buddy_table[i][tt+3] = '|';
      else buddy_table[i][tt+3] = ' ';
    }
    buddy_table[i][BUDDY_TABLE_COLS*BUDDY_TABLE_OFFSET+1] = '\0';
  }
  buddy_table_put_free_page();
  buddy_table_put_mem_page();
}

void buddy_table_show(){
  buddy_table_show_init();
  uart_puts((char *) "           0   1   2   3 | 4   5   6   7 | 8   9   a   b | c   d   e   f | 10 11  12  13 |14  15  16  17 |18  19  1a  1b |1c  1d  1e  1f |\n");
  uart_puts((char *) "===========================================================================================================================================\n");
  int row_offset = (BUDDY_PAGE_NUM%BUDDY_TABLE_COLS) ? 0 : -1;
  for (int i=0; i<BUDDY_TABLE_ROWS+row_offset; i++){
    uart_puts(buddy_table[i]);
    uart_puts((char *) "\n");
    if((i+1)%4 == 0){
  uart_puts((char *) "-------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
  }
}
