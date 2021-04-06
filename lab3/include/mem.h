# define MEM_TABLE_MAX_SIZE 1000

struct mem_node{
  unsigned long long addr;
  int bytes;
  int page_need;
  int idx;
  mem_node *pre;
  mem_node *next;
};

//struct mem_node *mem_inuse;

struct mem_node* get_mem_inuse();
void mem_init();
void mem_ll_show();
void*  malloc(int mbytes);
void free(void* addr);
