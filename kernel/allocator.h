#ifndef ALLOCATOR_H
#define ALLOCATOR_H

struct freelist_node{
    int index;
    struct freelist_node * prev;
    struct freelist_node * next;
};

struct pool_node_16{
    int num;
    int used[256];
    char *ptr;
    struct pool_node_16 * next;
};

struct pool_node_32{
    int num;
    int used[128];
    char *ptr;
    struct pool_node_32 * next;
};

struct pool_node_64{
    int num;
    int used[64];
    char *ptr;
    struct pool_node_64 * next;
};

struct pool_node_128{
    int num;
    int used[32];
    char *ptr;
    struct pool_node_128 * next;
};

struct buddy_block{
    int val;
    int used[16];
};

struct allocated_block{
    int index;
    int page_num;
};

enum list_val{
    EMPTY = -1
};

enum buddy_val{
    CONTIGUOUS = -1,
    ALLOCATED = -2
};

void push_list_tail(int num, int index);
int pop_list_head(int num);

void buddy_initialize();
void buddy_log_list();
void buddy_log_pool();
void buddy_log_allocated_table();

void * buddy_alloc(const int size);
void * buddy_contiguous_alloc(const int size);
void * buddy_pool_alloc(const int size);
void buddy_free(const int section);
void buddy_free_pool(const int pool, const int index);
void buddy_merge(const int list_index, const int block_index);
int buddy_divid_mem(const int list_index);
void buddy_mem_insufficient();

#endif

