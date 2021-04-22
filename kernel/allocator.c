#include "allocator.h"
#include "../lib/type.h"
#include "../lib/math.h"
#include "../lib/uart.h"
#include "../lib/string.h"

#define BUDDY_START_ADDR 0x10000000
#define BUDDY_END_ADDR 0x1f000000
#define BUDDY_LIST_START 0x1f000000
#define BUDDY_LIST_END 0x20000000
#define PAGE_SIZE 0x1000
#define ALLOCATED_TABLE_MAX 1000
#define BUDDY_LIST_SIZE 13
#define BUDDY_LIST_NODE_SIZE 0x100
#define FRAME_ARRAY_SIZE (BUDDY_END_ADDR - BUDDY_START_ADDR) / PAGE_SIZE

struct freelist_node buddy_list[BUDDY_LIST_SIZE];
struct buddy_block frame_array[FRAME_ARRAY_SIZE];
struct allocated_block allocated_table[ALLOCATED_TABLE_MAX];
struct pool_node_16 pool_16;
struct pool_node_32 pool_32;
struct pool_node_64 pool_64;
struct pool_node_128 pool_128;

int list_node_addr_counter = 0;

void buddy_log_list()
{
    char temp[10];
    for (int i = 0; i < BUDDY_LIST_SIZE; ++i)
    {
        uart_puts("  2^");
        itoa(i, temp, 0);
        uart_puts(temp);
        uart_puts(": ");
        
        if (buddy_list[i].index != EMPTY)
        {
            itoa(buddy_list[i].index, temp, 0);
            uart_puts(temp);
        }

        struct freelist_node * current = &buddy_list[i];
        while(current->next != NULL)
        {
            current = current->next;
            itoa(current->index, temp, 0);
            uart_puts("->");
            uart_puts(temp);
        }
        uart_puts("\n");
    }
}

void buddy_log_allocated_table()
{
    char temp[10];
    for (int i = 0; i < ALLOCATED_TABLE_MAX; ++i)
    {
        if (allocated_table[i].index != EMPTY)
        {
            uart_puts("\tmemory section ");
            itoa(i, temp, 0);
            uart_puts(temp);
            uart_puts(" in block [");
            itoa(allocated_table[i].index, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
            itoa(allocated_table[i].index + allocated_table[i].page_num, temp, 0);
            uart_puts(temp);
            uart_puts("]\n");
        }
    }
}

void buddy_log_pool()
{
    char temp[10];

    uart_puts("pool16: \n\t");
    for (int i = 0 ; i < pool_16.num; ++i)
    {
        if (pool_16.used[i] == 1)
        {
            uart_puts("");
            itoa(i, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
        }
    }
    uart_puts("used\n");

    uart_puts("pool32: \n\t");
    for (int i = 0 ; i < pool_32.num; ++i)
    {
        if (pool_32.used[i] == 1)
        {
            uart_puts("");
            itoa(i, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
        }
    }
    uart_puts("used\n");

    uart_puts("pool64: \n\t");
    for (int i = 0 ; i < pool_64.num; ++i)
    {
        if (pool_64.used[i] == 1)
        {
            uart_puts("");
            itoa(i, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
        }
    }
    uart_puts("used\n");

    uart_puts("pool128: \n\t");
    for (int i = 0 ; i < pool_128.num; ++i)
    {
        if (pool_128.used[i] == 1)
        {
            uart_puts("");
            itoa(i, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
        }
    }
    uart_puts("used\n");
}

void push_list_tail(int num, int index)
{
    struct freelist_node * node = (struct freelist_node *)((char)BUDDY_LIST_START + BUDDY_LIST_NODE_SIZE * list_node_addr_counter);
    struct freelist_node * current = &buddy_list[num];
    list_node_addr_counter++;
    char temp[10];
    uart_puts("\tOperation: Push index ");
    itoa(index, temp, 0);
    uart_puts(temp);
    uart_puts(" in list ");
    itoa(num, temp, 0);
    uart_puts(temp);
    uart_puts("\n");

    if (current->index == EMPTY)
    {
        current->index = index;
    }
    else
    {    
        while(current->next != NULL)
        {
            current = current->next;
        }

        current->next = node;
        current->next->index = index;
        current->next->prev = current;
        current->next->next = NULL;
    }
}

int pop_list_head(int num)
{
    struct freelist_node * current = &buddy_list[num];
    int index = current->index;

    char temp[10];
    uart_puts("\tOperation: Pop index ");
    itoa(index, temp, 0);
    uart_puts(temp);
    uart_puts(" in list ");
    itoa(num, temp, 0);
    uart_puts(temp);
    uart_puts("\n");
    
    if (current->next != NULL)
    {
        buddy_list[num] = *current->next;
        buddy_list[num].prev = NULL;
    }
    else
    {
        buddy_list[num].index = EMPTY;
    }

    return index;
}

void buddy_initialize()
{
    for (int i = 1; i < FRAME_ARRAY_SIZE; ++i)
    {
        frame_array[i].val = CONTIGUOUS;
    }

    for (int i = 0; i < BUDDY_LIST_SIZE; ++i)
    {
        buddy_list[i].index = EMPTY;
        buddy_list[i].prev = NULL;
        buddy_list[i].next = NULL;
    }
    for (int i = 0; i < FRAME_ARRAY_SIZE; i += pow(2, BUDDY_LIST_SIZE - 1))
    {
        push_list_tail(BUDDY_LIST_SIZE - 1, i);
        frame_array[i].val = BUDDY_LIST_SIZE - 1;
    }

    for (int i = 0; i < ALLOCATED_TABLE_MAX; ++i)
    {
        allocated_table[i].index = EMPTY;
        allocated_table[i].page_num = 0;
    }

    pool_16.ptr = buddy_contiguous_alloc(4096);
    pool_16.num = 256;
    for (int i = 0; i < pool_16.num; ++i)
    {
        pool_16.used[i] = 0;
    }

    pool_32.ptr = buddy_contiguous_alloc(4096);
    pool_32.num = 128;
    for (int i = 0; i < pool_32.num; ++i)
    {
        pool_32.used[i] = 0;
    }

    pool_64.ptr = buddy_contiguous_alloc(4096);
    pool_64.num = 64;
    for (int i = 0; i < pool_64.num; ++i)
    {
        pool_64.used[i] = 0;
    }

    pool_128.ptr = buddy_contiguous_alloc(4096);
    pool_128.num = 32;
    for (int i = 0; i < pool_128.num; ++i)
    {
        pool_128.used[i] = 0;
    }
}

void buddy_mem_insufficient()
{
    uart_puts("Error: Memeory is insufficient now.\n");
}

int buddy_divid_mem(const int list_index)
{
    int res = 0;
    int frame_index = buddy_list[list_index].index;
    int frame_index_next = frame_index + pow(2, frame_array[frame_index].val - 1);
    
    char str_size[10], str_index1[10], str_index2[10]; 
    itoa(list_index, str_size, 0);
    itoa(frame_index, str_index1, 0);    
    itoa(frame_index_next, str_index2, 0);
    uart_puts("Operation: Divid memory size 4K * 2^");
    uart_puts(str_size);
    uart_puts(" at block ");
    uart_puts(str_index1);
    uart_puts(", ");
    uart_puts(str_index2);
    uart_puts("\n");

    pop_list_head(list_index);
    push_list_tail(list_index - 1, frame_index);
    push_list_tail(list_index - 1, frame_index_next);
    
    frame_array[frame_index].val--;
    frame_array[frame_index_next].val = frame_array[frame_index].val;

    res = list_index - 1;

    return res;
}

void * buddy_alloc(const int size)
{
    if (size > 128)
    {
        return buddy_contiguous_alloc(size);
    }
    else
    {
        return buddy_pool_alloc(size);
    }
}

void * buddy_pool_alloc(const int size)
{
    // 16, 32, 64, 128
    if (size <= 16)
    {
        struct pool_node_16 * current = &pool_16;
        
        while (1)
        {
            while(current != NULL)
            {
                for (int i = 0; i < current->num; ++i)
                {
                    if (current->used[i] == 0)
                    {
                        current->used[i] = 1;
                        return current->ptr + 16 * i;
                    }
                }
                current = current->next;
            }
            
            current->ptr = buddy_contiguous_alloc(4096);
            current->num = 256;
            for (int i = 0; i < current->num; ++i)
            {
                current->used[i] = 0;
            }
        }
    }
    else if (size <= 32)
    {
        struct pool_node_32 * current = &pool_32;
        
        while (1)
        {
            while(current != NULL)
            {
                for (int i = 0; i < current->num; ++i)
                {
                    if (current->used[i] == 0)
                    {
                        current->used[i] = 1;
                        return current->ptr + 32 * i;
                    }
                }
                current = current->next;
            }
            current->ptr = buddy_contiguous_alloc(4096);
            current->num = 128;
            for (int i = 0; i < current->num; ++i)
            {
                current->used[i] = 0;
            }
        }
    }
    else if (size <= 64)
    {
        struct pool_node_64 * current = &pool_64;
        
        while (1)
        {
            while (current != NULL)
            {
                for (int i = 0; i < current->num; ++i)
                {
                    if (current->used[i] == 0)
                    {
                        current->used[i] = 1;
                        return current->ptr + 64 * i;
                    }
                }
                current = current->next;
            }
            current->ptr = buddy_contiguous_alloc(4096);
            current->num = 64;
            for (int i = 0; i < current->num; ++i)
            {
                current->used[i] = 0;
            }
        }
    }
    else if (size <= 128)
    {
        struct pool_node_128 * current = &pool_128;
        
        while (1)
        {
            while(current != NULL)
            {
                for (int i = 0; i < current->num; ++i)
                {
                    if (current->used[i] == 0)
                    {
                        current->used[i] = 1;
                        return current->ptr + 128 * i;
                    }
                }
                current = current->next;
            }
            
            current->ptr = buddy_contiguous_alloc(4096);
            current->num = 32;
            for (int i = 0; i < current->num; ++i)
            {
                current->used[i] = 0;
            }
        }
    }
    return NULL;
}

void buddy_free_pool(const int pool, const int index)
{
    if (pool == 16)
    {
        pool_16.used[index] = 0;
    }
    else if (pool == 32)
    {
        pool_32.used[index] = 0;
    }
    else if (pool == 64)
    {
        pool_64.used[index] = 0;
    }
    else if (pool == 128)
    {
        pool_128.used[index] = 0;
    }
}

void * buddy_contiguous_alloc(const int size)
{
    // 4k per unit
    int alloc_page_num = (size + 4095) / 4096;
    int target_list_index = -1;

    void * ptr = 0x0000;

    for (int i = 0; i < BUDDY_LIST_SIZE; ++i)
    {
        if (pow(2, i) < alloc_page_num)
        {
            continue;
        }
        if (buddy_list[i].index != EMPTY)
        {
            target_list_index = i;
            break;
        }
    }

    if (target_list_index == -1)
    {
        buddy_mem_insufficient();
        return NULL;
    }

    while(alloc_page_num * 2 <= pow(2, target_list_index))
    {
        target_list_index = buddy_divid_mem(target_list_index);
    }

    int target_block_index = pop_list_head(target_list_index);
    for (int i = 0; i < pow(2, target_list_index); ++i)
    {
        frame_array[target_block_index + i].val = ALLOCATED;
    }
    ptr = (char*)BUDDY_START_ADDR + target_block_index * PAGE_SIZE;

    for (int i = 0; i < ALLOCATED_TABLE_MAX; ++i)
    {
        if (allocated_table[i].index == EMPTY)
        {
            allocated_table[i].index = target_block_index;
            allocated_table[i].page_num = pow(2, target_list_index);

            break;
        }
    }

    char temp[10];
    uart_puts("Operation: Block [");
    itoa(target_block_index, temp, 0);
    uart_puts(temp);
    uart_puts(", ");
    itoa(target_block_index + pow(2, target_list_index), temp, 0);
    uart_puts(temp);
    uart_puts("] at is allocated\n");

    return ptr;
}

void buddy_free(const int section)
{
    if (section >= ALLOCATED_TABLE_MAX)
    {
        uart_puts("Error: Exceed Section Max Number!\n");
        return;
    }

    if (allocated_table[section].index == EMPTY)
    {
        char temp[10];
        uart_puts("Warning: Memory Section ");
        itoa(section, temp, 0);
        uart_puts(" is already free memory.\n");
        return;
    }

    int block_index = allocated_table[section].index;
    int page_num = allocated_table[section].page_num;
    int list_index = log(2, page_num);

    allocated_table[section].index = EMPTY;
    allocated_table[section].page_num = 0;

    for (int i = 0; i < page_num; ++i)
    {
        frame_array[block_index + i].val = CONTIGUOUS;
    }
    frame_array[block_index].val = list_index;

    buddy_merge(list_index, block_index);
}

void buddy_merge(const int list_index, const int block_index)
{
    struct freelist_node * current = &buddy_list[list_index];
    int label = 0;

    if (list_index + 1 > BUDDY_LIST_SIZE - 1) 
    {
        push_list_tail(list_index, block_index);
        return;
    }

    if (block_index % pow(2, list_index + 1) == 0) label = 0; // right
    else label = 1; // left

    while (current != NULL)
    {
        if (label == 0 && current->index == block_index + pow(2, list_index))
        {
            // right
            frame_array[block_index].val = list_index + 1;
            frame_array[block_index + pow(2, list_index)].val = CONTIGUOUS;

            char temp[10];
            uart_puts("  Operation: Merge block [");
            itoa(block_index, temp, 0);
            uart_puts(temp);
            uart_puts(", ");
            itoa(block_index + pow(2, list_index), temp, 0);
            uart_puts(temp);
            uart_puts("]\n");

            if (current->prev != NULL) current->prev->next = current->next;
            else pop_list_head(list_index);
            buddy_merge(list_index + 1, block_index);
            return;  
        }
        else if (label == 1 && current->index == block_index - pow(2, list_index))
        {
            // left
            frame_array[block_index].val = CONTIGUOUS;
            frame_array[block_index - pow(2, list_index)].val = CONTIGUOUS;

            char temp[10];
            uart_puts("  Operation: Merge block [");
            itoa(block_index - pow(2, list_index), temp, 0);
            uart_puts(temp);
            uart_puts(", ");
            itoa(block_index, temp, 0);
            uart_puts(temp);
            uart_puts("]\n");

            if (current->prev != NULL) current->prev->next = current->next;
            else pop_list_head(list_index);
            buddy_merge(list_index + 1, block_index - pow(2, list_index));
            return;
        }

        current = current->next;
    }

    push_list_tail(list_index, block_index);
}