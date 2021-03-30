# include "buddy.h"
# include "uart.h"
# include "my_math.h"
# include "linklist.h"
# include "linklist.c"

char buddy_table[BUDDY_TABLE_ROWS][BUDDY_TABLE_COLS*BUDDY_TABLE_OFFSET+BUDDY_TABLE_OFFSET+2];
struct buddy_node buddy_ll[BUDDY_LL_MAX_NUM];
struct buddy_node *buddy_head[BUDDY_MAX_ORDER+1];
struct buddy_node *buddy_unuse_head;

inline int buddy_addr_to_pn(unsigned long long addr){
  return (int)(addr-BUDDY_BASE_ADDR)/BUDDY_PAGE_SIZE;
}

inline unsigned long long buddy_pn_to_addr(int pn){
  return (unsigned long long)pn*BUDDY_PAGE_SIZE+BUDDY_BASE_ADDR;
}

void buddy_uart_puts(char *c, int it){
  for (int i=0; i<it; i++){
    uart_puts("  ");
  }
  uart_puts(c);
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

  unsigned long long buddy_assign_t = BUDDY_BASE_ADDR;
  while(buddy_assign_t < BUDDY_BASE_ADDR+BUDDY_SIZE){
    struct buddy_node *new_node = ll_pop_front<struct buddy_node>(&buddy_unuse_head);
    new_node->order = BUDDY_MAX_ORDER;
    new_node->addr = buddy_assign_t;
    buddy_assign_t += (1 << BUDDY_MAX_ORDER)*BUDDY_PAGE_SIZE;
    ll_push_back<struct buddy_node>(&buddy_head[BUDDY_MAX_ORDER], new_node);
    //bnll_push_end(&buddy_head[BUDDY_MAX_ORDER], new_node);
  }
}

void buddy_push_loc(struct buddy_node **head, struct buddy_node *node, int itrn){
  char ct[20];
  buddy_uart_puts("Push loc, addr <", itrn);
  int_to_hex(node->addr, ct);
  uart_puts(ct);
  uart_puts("> into order");
  int_to_str(node->order, ct);
  uart_puts(ct);
  uart_puts("\n");
  struct buddy_node *head_t = *head;
  if (!head_t){
    *head = node;
    node->next = 0;
    node->pre = 0;
    buddy_uart_puts("return\n", itrn);
    return ;
  }
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
        del_node = head_t;
        node->order ++;
      }
    }
    //del this order linklist
    if(del_node){
      buddy_uart_puts("Find addr<", itrn+1);
      int_to_hex(del_node->addr, ct);
      uart_puts(ct);
      uart_puts("> can coalesce.\n");
      ll_rm_elm<struct buddy_node>(&buddy_head[order], del_node);
      ll_push_front<struct buddy_node>(&buddy_unuse_head, del_node);
      buddy_push_loc(&buddy_head[order+1], node, itrn+1);
    }
    else{
      ll_push_elm<struct buddy_node>(&buddy_head[order], node, head_t);
    }
    /*
    int coalesce_flag = 0;
    if(npis == hpis){
      coalesce_flag = 1;
      node->addr = head_t->addr;
    }
    else if(npis == hnpis){
      coalesce_flag = 1;
      head_t = head_t->next;
    }
    if (coalesce_flag == 1){
      struct buddy_node *del_node = head_t;
      if(!head_t->pre){
        *head = head_t->next;
        head_t->next->pre = 0;
      }
      else if(!head_t->next){
        head_t->pre->next = 0;
      }
      ll_push_front(&buddy_unuse_head, del_node);
      node->order++;
      buddy_push_loc(&buddy_head[order+1], node, itrn+1);
    }
    
    else{
      ll_push_elm<strucy buddy_node>(&buddy_head[order], node, head_t);
      //head_t->pre->next = head_t->next;
      //head_t->next->pre = head_t->pre;
    }
    */
  }
  else{
    ll_push_elm<struct buddy_node>(&buddy_head[order], node, head_t);
    //node->next = head_t->next;
    //node->pre = head_t;
    //node->next->pre = node;
    //node->pre->next = node;
  }
  buddy_uart_puts("return\n", itrn);
}

unsigned long long buddy_alloc(int mbytes, int order, int itrn){
  char ct[20];
  buddy_uart_puts("Alloc ", itrn);
  uart_puts("Order : ");
  int_to_str(order, ct);
  uart_puts(ct);
  uart_puts("\n");
  if(!buddy_head[order]){
    buddy_uart_puts("This order is null\n", itrn+1);
    unsigned long long r;
    if (order >= BUDDY_MAX_ORDER) r = 0;
    else r = buddy_alloc(mbytes, order+1, itrn+1);
    buddy_uart_puts("return\n", itrn);
    return r;
  }
  buddy_uart_puts("Found free page.\n", itrn+1);
  struct buddy_node *alloc_node= ll_pop_front<struct buddy_node>(&buddy_head[order]);
  unsigned long long r = alloc_node->addr;
  int page_need = mbytes/BUDDY_PAGE_SIZE;
  page_need = (mbytes%BUDDY_PAGE_SIZE == 0) ? page_need : page_need+1;
  buddy_uart_puts("Page need = ", itrn+1);
  int_to_str(page_need, ct);
  uart_puts(ct);
  int page_redundant = (1 << order) - page_need;
  uart_puts(", Pageredundant = ");
  int_to_str(page_redundant, ct);
  uart_puts(ct);
  uart_puts("\n");
  int prnp = buddy_addr_to_pn(r)+page_need; //page redundant, next page
  int prl_order = 0; //page redundant last order
  while(page_redundant){
    if(page_redundant%2){
      struct buddy_node *new_node = ll_pop_front<struct buddy_node>(&buddy_unuse_head);
      new_node->order = prl_order;
      new_node->addr = buddy_pn_to_addr(prnp);
      buddy_push_loc(&buddy_head[prl_order], new_node, itrn+1);
      prnp += (1 << prl_order);
    }
    page_redundant /= 2;
    prl_order++;
  }
  //
  alloc_node->order = -1;
  ll_push_front(&buddy_unuse_head, alloc_node);
  buddy_uart_puts("return\n", itrn);
  return r;
}

char* buddy_table_get_char(int pn){
  int row = pn/BUDDY_TABLE_COLS;
  int col = pn%BUDDY_TABLE_COLS;
  char *r = &buddy_table[row][col*BUDDY_TABLE_TIMES+BUDDY_TABLE_OFFSET+1];
  return r;
}

void buddy_ll_show(){
  uart_puts("unuse_head : ");
  char tt[3];
  int_to_str(buddy_unuse_head->idx, tt);
  uart_puts(tt);
  uart_puts("\n");
  for (int i=0; i<BUDDY_MAX_ORDER+1; i++){
    int_to_str(i, tt);
    uart_puts("Order ");
    uart_puts(tt);
    uart_puts(":\n");
    if (buddy_head[i]){
      struct buddy_node *t = buddy_head[i];
      while(t){
        char ttt[20];
        int_to_hex(t->addr, ttt);
        uart_puts("   ");
        uart_puts(ttt);
        t = t->next;
      }
      uart_puts("\n");
    }
    else{
      uart_puts("  null\n");
    }
  }
}

void buddy_table_put_free_page(){
  for (int i=0; i<BUDDY_MAX_ORDER+1; i++){
    if (buddy_head[i]){
      struct buddy_node *t = buddy_head[i];
      while(t){
        int page_num = (1 << t->order);
        int page_addr = (int)( (t->addr-BUDDY_BASE_ADDR)/BUDDY_PAGE_SIZE );
        for (int j=0; j<page_num; j++){
          char *tt = buddy_table_get_char(page_addr++);
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
}

void buddy_table_show(){
  buddy_table_show_init();
  uart_puts("Hi\n");
  uart_puts("           0   1   2   3 | 4   5   6   7 | 8   9   a   b | c   d   e   f | 10 11  12  13 |14  15  16  17 |18  19  1a  1b |1c  1d  1e  1f |\n");
  uart_puts("===========================================================================================================================================\n");
  for (int i=0; i<BUDDY_TABLE_ROWS; i++){
    uart_puts(buddy_table[i]);
    uart_puts("\n");
    if((i+1)%4 == 0){
  uart_puts("-------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
  }
}
