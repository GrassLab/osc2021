# ifndef BUDDY
# define BUDDY

# include "bitset.h"
# include "page.h"
# include "mem_addr.h"

# define BUDDY_BASE_ADDR   (0x10000000 + KVA)
# define BUDDY_SIZE        (0x800000)
# define BUDDY_PAGE_SIZE   PAGE_SIZE
# define BUDDY_PAGE_NUM    (BUDDY_SIZE/BUDDY_PAGE_SIZE)
# define BUDDY_MAX_ORDER   12
# define BUDDY_LL_MAX_NUM  (BUDDY_PAGE_NUM/2+10)

# define BUDDY_TABLE_COLS   32
# define BUDDY_TABLE_TIMES  4
# define BUDDY_TABLE_OFFSET 19
# define BUDDY_TABLE_ROWS   (BUDDY_PAGE_NUM/BUDDY_TABLE_COLS+1)

# define BUDDY_DMA_UNIT_SIZE    16
# define BUDDY_DMA_LEVEL_THRE   256   //
# define BUDDY_DMA_BITSET_LEN   ((int)((BUDDY_PAGE_SIZE/BUDDY_DMA_UNIT_SIZE)/INTBITS)+1)
# define BUDDY_DMA_LL_MAX_SIZE  300
# define BUDDY_DMA_SLOT_NUM (BUDDY_DMA_LEVEL_THRE/BUDDY_DMA_UNIT_SIZE+BUDDY_PAGE_SIZE/BUDDY_DMA_LEVEL_THRE)

struct buddy_node{
  int idx;
  int order;
  unsigned long long addr;
  struct buddy_node *pre;
  struct buddy_node *next;
};

struct dma_node{
  int idx;
  int chunk_size;
  int bitset[BUDDY_DMA_BITSET_LEN];
  int free_chunk_num;
  int max_chunk_num;
  unsigned long long addr;
  struct dma_node *pre;
  struct dma_node *next;
};

//inline int buddy_addr_to_pn(unsigned long long addr);
//inline long long int buddy_pn_to_addr(int pn);
void buddy_dma_init();
unsigned long long buddy_dma_alloc(int mbytes, int itrn);
void buddy_dma_free(unsigned long long base_addr, int mbytes, int itrn);
void buddy_init();
char* buddy_table_get_char(int pn);
unsigned long long buddy_alloc(int mbytes, int order, int itrn);
void buddy_free(unsigned long long base_addr, int mbytes, int itrn);
void buddy_ll_show();
void buddy_dma_ll_show();
//void buddy_table_draw();
void buddy_table_show();

# endif
