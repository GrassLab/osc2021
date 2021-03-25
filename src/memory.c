#include "printf.h"
#include "memory.h"
#include "math.h"

struct page bookkeep[MAX_PAGE_NUMBER];
struct list_head free_buddy_list[MAX_BUDDY_ORDER + 1]; // 0 ~ 9, so 10 elements in total
struct object_allocator allocator_pool[MAX_ALLOCATOR_NUMBER];

void init_buddy()
{
    for (int i = 0; i < MAX_PAGE_NUMBER; i++)
    {
        bookkeep[i].order = -1;
        bookkeep[i].page_number = i;
        bookkeep[i].used = 0;
        bookkeep[i].address = (void *)MEMORY_START + i * PAGE_SIZE;

        // list_init_head(&(bookkeep[i].list));
    }

    // 0 ~ 9
    for (int i = 0; i < MAX_BUDDY_ORDER + 1; i++)
        list_init_head(&(free_buddy_list[i]));

    // having the address of bookkeep[i].list is equal to having the address of bookkeep[i]
    // thus we can get the address of the page from the list node
    for (int i = 0; i < MAX_PAGE_NUMBER; i += MAX_BLOCK_SIZE)
    {
        bookkeep[i].order = MAX_BUDDY_ORDER;
        list_add_tail(&bookkeep[i].list, &free_buddy_list[MAX_BUDDY_ORDER]);
    }
}

void init_object_allocator()
{
    struct object_allocator *allocator;

    for (int i = MIN_OBJECT_ORDER; i < MAX_OBJECT_ORDER + 1; i++)
    {
        allocator = &allocator_pool[i - MIN_OBJECT_ORDER];

        allocator->current_page = NULL;
        allocator->object_size = (1 << i);
        list_init_head(&allocator->full);
        list_init_head(&allocator->partial);
        allocator->preserved_empty = NULL;
    }
}

void init_memory()
{
    init_buddy();
    init_object_allocator();
}

struct page* block_allocation(int order)
{
    if ((order > MAX_BUDDY_ORDER) || (order < 0))
    {
        printf("[block_allocation] invalid order!\n\n");
        return 0;
    }

    for (int current_order = order; current_order < MAX_BUDDY_ORDER + 1; current_order++)
    {
        // go to next order
        if (list_empty(&free_buddy_list[current_order]))
            continue;

        // free block found
        struct page *temp_block = (struct page *)free_buddy_list[current_order].next;
        list_crop(&temp_block->list, &temp_block->list);
        temp_block->used = 1;
        temp_block->order = order;

        while(current_order > order)
        {
            current_order--;

            int bottom_page_number = find_buddy(temp_block->page_number, current_order);
            struct page *bottom = &bookkeep[bottom_page_number];
            bottom->order = current_order;
            
            list_add_tail(&bottom->list, &free_buddy_list[current_order]);
            
            printf("[block_allocation] redundant block(page: %d, order: %d) freed\n", 
                    bottom->page_number, bottom->order);
        }

        printf("[block_allocation] block(page: %d, order: %d) allocated\n", 
                temp_block->page_number, temp_block->order);
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

    if (block->used == 0)
    {
        printf("[block_free] this block is already freed!\n");
        return;
    }

    printf("[block_free] block(page: %d, order: %d) to be freed\n",
            block->page_number, block->order);

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

        printf("[block_free] block(page: %d, order: %d) and block(page: %d, order: %d) merged\n",
               top->page_number, top->order, bottom->page_number, bottom->order);

        bottom->order = -1;
        top->order++;

        // next iteration
        block = top;
        buddy_page_number = find_buddy(block->page_number, block->order);
        buddy = &bookkeep[buddy_page_number];
    }

    // stop merge
    list_add_tail(&block->list, &free_buddy_list[block->order]);
    printf("[block_free] done\n\n");
}

void *object_allocation(int token)
{
    struct object_allocator *allocator = &allocator_pool[token];

    if(allocator->current_page == NULL)
    {
        struct page *temp_page;

        // fill up pages with free space first
        if (!list_empty(&allocator->partial))
        {
            temp_page = (struct page *)allocator->partial.next;
            list_crop(&temp_page->list, &temp_page->list);
        }
        // use the preserved empty page if there is one
        else if (&allocator->preserved_empty != NULL)
        {
            temp_page = (struct page *)allocator->preserved_empty;
            list_crop(&temp_page->list, &temp_page->list);
        }
        // demand a new page
        else
        {
            temp_page = block_allocation(0);

            temp_page->allocator = allocator;
            for (int i = 0; i < MAX_OBJECT_IN_A_PAGE; i++)
                temp_page->hole_used[i] = 0;
            temp_page->object_count = 0;
            temp_page->max_object_count = PAGE_SIZE / (2 << (token + MIN_OBJECT_ORDER));
        }
        allocator->current_page = temp_page;
    }

    int index = 0;
    struct page *current_page = allocator->current_page;
    // look for a hole to fit in
    for (int i = 0; i < current_page->max_object_count; i++)
    {
        if (current_page->hole_used[i] == 0)
        {
            index = i;
            current_page->hole_used[i] = 1;
            current_page->object_count++;
            break;
        }
    }

    // the page is full now
    if (current_page->object_count == current_page->max_object_count)
    {
        list_add_tail(&current_page->list, &allocator->full);
        allocator->current_page = NULL;
    }

    void *object = current_page->address + index * allocator->object_size;

    printf("[object_allocation] object(page: %d, size: %d, index: %d)\n"
            , current_page->page_number, allocator->object_size, index);
    printf("[object_allocation] done\n\n");

    return object;
}

void object_free(void *object)
{
    int page_number = (long)(object - MEMORY_START) >> PAGE_SHIFT;
    struct page *page = &bookkeep[page_number];
    struct object_allocator *allocator = page->allocator;
    // for example, if we have 16384 + 4096 * 14 + 32 * 5 as our address, then we get 5 with the following operation
    int index = (((long)(object - MEMORY_START) & ((1 << PAGE_SHIFT) - 1)) / allocator->object_size);

    if (page->hole_used[index] == 0)
    {
        printf("[object_free] this object is already freed!\n");
        return;
    }

    page->hole_used[index] = 0;
    page->object_count--;

    if (page != allocator->current_page)
    {
        list_crop(&page->list, &page->list);
        // full to partial
        if (page->object_count > 0)
            list_add_tail(&page->list, &allocator->partial);
        // partial to empty
        else
        {
            if (allocator->preserved_empty == NULL)
                allocator->preserved_empty = page;
            else
                block_free(page);
        }
    }

    printf("[object_free] object(page: %d, size: %d, index: %d)\n", page->page_number, allocator->object_size, index);
    printf("[object_free] done\n\n");
}

void *memory_allocation(int size)
{
    void *address;

    if (size <= (PAGE_SIZE / 2))
    {
        for (int i = MIN_OBJECT_ORDER; i < MAX_OBJECT_ORDER + 1; i++)
        {
            if (size <= (1 << i))
            {
                address = object_allocation(i - MIN_OBJECT_ORDER);
                return address;
            }
        }
    }
    else
    {
        for (int i = 0; i < MAX_BUDDY_ORDER; i++)
        {
            if (size <= (1 << (i + PAGE_SHIFT)))
            {
                address = block_allocation(i)->address;
                return address;
            }
        }
    }

    printf("[memory_allocation] the requested size is too large!\n");
    return 0;
}

void memory_free(void *address)
{
    int page_number = (long)(address - MEMORY_START) >> PAGE_SHIFT;
    struct page *page = &bookkeep[page_number];

    if (page->allocator)
        object_free(address);
    else
        block_free(page);
}

int find_buddy(int page_number, int order)
{
    return page_number ^ (1 << order);
}