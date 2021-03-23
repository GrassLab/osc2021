#include "printf.h"
#include "memory.h"
#include "math.h"

struct page bookkeep[MAX_PAGE_NUMBER];
struct list_head free_buddy_list[MAX_ORDER + 1]; // 0 ~ 9, so 10 elements in total

void init_buddy()
{
    for (int i = 0; i < MAX_PAGE_NUMBER; i++)
    {
        bookkeep[i].order = -1;
        bookkeep[i].page_number = i;
        bookkeep[i].used = 0;
        bookkeep[i].address = (void *)MEMORY_START + i * PAGE_SIZE;

        list_init_head(&(bookkeep[i].list));
    }

    // 0 ~ 9
    for (int i = 0; i < MAX_ORDER + 1; i++)
        list_init_head(&(free_buddy_list[i]));

    // having the address of bookkeep[i].list is equal to having the address of bookkeep[i]
    // thus we can get the address of the page from the list node
    for (int i = 0; i < MAX_PAGE_NUMBER; i += MAX_SIZE_IN_PAGE)
        list_add_tail(&bookkeep[i].list, &free_buddy_list[MAX_ORDER]);
}

struct page* block_allocation(int order)
{
    if ((order > MAX_ORDER) || (order < 0))
    {
        printf("[block_allocation] invalid order!\n\n");
        return 0;
    }

    for (int current_order = order; current_order < MAX_ORDER + 1; current_order++)
    {
        // go to next order
        if (list_empty(&free_buddy_list[current_order]))
            continue;

        // printf("%d\n", current_order);

        // free block found
        struct page *temp_block = (struct page *)free_buddy_list[current_order].next;
        list_crop(&temp_block->list, &temp_block->list);
        temp_block->used = 1;
        temp_block->order = order;

        while(current_order > order)
        {
            current_order--;

            // printf("%d %d\n", temp_block->page_number, current_order);

            int bottom_page_number = find_buddy(temp_block->page_number, current_order);
            struct page *bottom = &bookkeep[bottom_page_number];
            bottom->order = current_order;
            
            list_add_tail(&bottom->list, &free_buddy_list[current_order]);
            
            printf("[block_allocation] redundant block(number: %d, order: %d) freed\n", bottom->page_number, bottom->order);
        }

        printf("[block_allocation] block(number: %d, order: %d) allocated\n", temp_block->page_number, temp_block->order);
        printf("[block_allocation] done\n\n");
        return temp_block;
    }
    printf("[block_allocation] no free space!\n\n");
    return 0;
}

void block_free(struct page *block)
{
    struct page *buddy, *top, *bottom;
    int buddy_page_number;

    printf("[block_free] block(number: %d, order: %d) to be freed\n", block->page_number, block->order);

    block->used = 0;
    buddy_page_number = find_buddy(block->page_number, block->order);
    buddy = &bookkeep[buddy_page_number];

    // iterate if buddy can be merged
    while(buddy->order == block->order && !buddy->used)
    {
        list_crop(&buddy->list, &buddy->list);
        if (buddy->page_number > block->page_number)
        {
            top = block;
            bottom = buddy;
        }
        else
        {
            top = buddy;
            bottom = block;
        }

        printf("[block_free] block(number: %d, order: %d) and block(number: %d, order: %d) merged\n",
               top->page_number, top->order, bottom->page_number, bottom->order);

        bottom->order = -1;
        top->order += 1;

        // next iteration
        block = top;
        buddy_page_number = find_buddy(block->page_number, block->order);
        buddy = &bookkeep[buddy_page_number];
    }

    // stop merge
    list_add_tail(&block->list, &free_buddy_list[block->order]);
    printf("[block_free] done\n\n");
}

int find_buddy(int page_number, int order)
{
    return page_number ^ (1 << order);
}