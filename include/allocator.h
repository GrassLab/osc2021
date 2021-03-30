#define BASE_MEM 0x1000000
/* max allocation size is 2 GB */
#define MAX_ALLOC_LOG2 20
#define MAX_ALLOC_SIZE (1 << MAX_ALLOC_LOG2)
/* each page is 16 bytes */
#define PAGE_SIZE_LOG2 4
#define PAGE_SIZE (1 << PAGE_SIZE_LOG2)
#define PAGE_NUM (1  << (MAX_ALLOC_LOG2 - PAGE_SIZE_LOG2))


#define BUCKETS_NUM (MAX_ALLOC_LOG2 - PAGE_SIZE_LOG2 + 1)
#define NULL ((void*)0)


/* linked list for blocks */
typedef struct node {
    int index;
    struct node *next;
} Node;

void* malloc(unsigned int size);
void free_page(unsigned long addr, unsigned int size);
void init_buckets();