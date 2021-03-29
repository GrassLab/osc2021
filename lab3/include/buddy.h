# define BUDDY_BASE_ADDR   0x100000000
# define BUDDY_SIZE        0x400000
# define BUDDY_PAGE_SIZE   0x1000
# define BUDDY_PAGE_NUM    BUDDY_SIZE/BUDDY_PAGE_SIZE
# define BUDDY_MAX_ORDER   10
# define BUDDY_LL_MAX_NUM  BUDDY_PAGE_NUM/2+10

# define BUDDY_TABLE_COLS 32
# define BUDDY_TABLE_TIMES 4
# define BUDDY_TABLE_OFFSET 9
# define BUDDY_TABLE_ROWS BUDDY_PAGE_NUM/BUDDY_TABLE_COLS

struct buddy_node{
  int idx;
  int order;
  unsigned long long addr;
  struct buddy_node *pre;
  struct buddy_node *next;
};


void buddy_init();
char* buddy_table_get_char(int pn);
void buddy_ll_show();
//void buddy_table_draw();
void buddy_table_show();
