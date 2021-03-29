# include "buddy.h"
# include "uart.h"
# include "my_math.h"

char buddy_table[BUDDY_TABLE_ROWS][BUDDY_TABLE_COLS*BUDDY_TABLE_OFFSET+BUDDY_TABLE_OFFSET+2];
struct buddy_node buddy_ll[BUDDY_LL_MAX_NUM];
struct buddy_node *buddy_head[BUDDY_MAX_ORDER+1];
struct buddy_node *buddy_unuse_head;

struct buddy_node* bnll_pop(struct buddy_node **llh){
  struct buddy_node *r = *llh;
  r->next->pre = 0;
  *llh = r->next;
  r->next = 0;
  r->pre = 0;
  return r;
}

void bnll_push(struct buddy_node **llh, struct buddy_node *node){
  struct buddy_node *head = *llh;
  if(head){
    node->next = head;
    node->pre = 0;
    head->pre = node;
    (*llh) = node;
  }
  else{
    node->next = 0;
    node->pre = 0;
    (*llh) = node;
  }
}

void bnll_push_end(struct buddy_node **llh, struct buddy_node *node){
  struct buddy_node *head = *llh;
  if (head){
    while(head->next){
      head = head->next;
    }
    node->next = 0;
    node->pre = head;
    head->next = node;
  }
  else{
    node->next = 0;
    node->pre = 0;
    (*llh) = node;
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

  unsigned long long buddy_assign_t = BUDDY_BASE_ADDR;
  while(buddy_assign_t < BUDDY_BASE_ADDR+BUDDY_SIZE){
    struct buddy_node *new_node = bnll_pop(&buddy_unuse_head);
    new_node->order = BUDDY_MAX_ORDER;
    new_node->addr = buddy_assign_t;
    buddy_assign_t += (1 << BUDDY_MAX_ORDER)*BUDDY_PAGE_SIZE;
    bnll_push_end(&buddy_head[BUDDY_MAX_ORDER], new_node);
  }
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
      uart_puts("  is null\n");
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
