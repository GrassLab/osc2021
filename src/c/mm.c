#include "printf.h"
#include "mm.h"
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
        bookkeep[i].start_address = (void *)MEMORY_START + i * PAGE_SIZE;
        bookkeep[i].allocator = NULL;

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
        allocator->max_object_count = PAGE_SIZE / (1 << i);

        list_init_head(&allocator->full);
        list_init_head(&allocator->partial);
        // allocator->preserved_empty = NULL;
    }
}

void init_memory()
{
    init_buddy();
    init_object_allocator();
}

struct page *block_allocation(int order)
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

        while (current_order > order)
        {
            current_order--;

            int bottom_page_number = find_buddy(temp_block->page_number, current_order);
            struct page *bottom = &bookkeep[bottom_page_number];
            bottom->order = current_order;

            list_add_tail(&bottom->list, &free_buddy_list[current_order]);

            // printf("[block_allocation] redundant block(page: %d, order: %d) freed\n",
                //    bottom->page_number, bottom->order);
        }

        // printf("[block_allocation] block(page: %d, order: %d) allocated\n",
            //    temp_block->page_number, temp_block->order);
        // printf("[block_allocation] done\n\n");

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

    // printf("[block_free] block(page: %d, order: %d) to be freed\n",
        //    block->page_number, block->order);

    block->used = 0;
    // remember to clean the point to the allocator !!!
    block->allocator = NULL;

    buddy_page_number = find_buddy(block->page_number, block->order);
    buddy = &bookkeep[buddy_page_number];

    // iterate if buddy can be merged
    while (buddy->order == block->order && buddy->order < MAX_BUDDY_ORDER && !buddy->used)
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

        // printf("[block_free] block(page: %d, order: %d) and block(page: %d, order: %d) merged\n",
            //    top->page_number, top->order, bottom->page_number, bottom->order);

        bottom->order = -1;
        top->order++;

        // next iteration
        block = top;
        buddy_page_number = find_buddy(block->page_number, block->order);
        buddy = &bookkeep[buddy_page_number];
    }

    // stop merge
    list_add_tail(&block->list, &free_buddy_list[block->order]);
    // printf("[block_free] done\n\n");
}

void *object_allocation(int token)
{
    struct object_allocator *allocator = &allocator_pool[token];

    if (allocator->current_page == NULL)
    {
        struct page *temp_page;

        // fill up pages with free space first
        if (!list_empty(&allocator->partial))
        {
            temp_page = (struct page *)allocator->partial.next;
            list_crop(&temp_page->list, &temp_page->list);
        }
        // // use the preserved empty page if there is one
        // else if (allocator->preserved_empty != NULL)
        // {
        //     temp_page = allocator->preserved_empty;
        //     allocator->preserved_empty = NULL;
        // }
        // demand a new page
        else
        {
            temp_page = block_allocation(0);

            temp_page->allocator = allocator;
            temp_page->object_count = 0;

            // first_free points to the starting address of the page when allocated
            temp_page->first_free = temp_page->start_address;
            // block i saves the offset number for block i+1
            for (int i = 0; i < allocator->max_object_count; i++)
                *(int *)(temp_page->start_address + i * allocator->object_size) =
                        (i + 1) * allocator->object_size;
        }

        allocator->current_page = temp_page;
    }

    struct page *current_page = allocator->current_page;
    void *object = current_page->first_free;

    // if first_free points to 0x8000 now, and it stores 64, 
    // which is the location to the next free block behind first_free,
    // then first_free will point to 0x8040 afterward 
    current_page->first_free = current_page->start_address + 
                               *(int *)(current_page->first_free);
    current_page->object_count++;

    // the page is full now
    if (current_page->object_count == allocator->max_object_count)
    {
        list_add_tail(&current_page->list, &allocator->full);
        allocator->current_page = NULL;
    }

    // int index = (object - current_page->start_address) / allocator->object_size;

    // printf("[object_allocation] object(page: %d, size: %d, index: %d) allocated\n", current_page->page_number, allocator->object_size, index);
    // printf("[object_allocation] done\n\n");

    return object;
}

void object_free(void *object)
{
    int page_number = (long)(object - MEMORY_START) >> PAGE_SHIFT;
    struct page *page = &bookkeep[page_number];
    struct object_allocator *allocator = page->allocator;
    // for example, if we have 16384 + 4096 * 14 + 32 * 5 as our address, then we get 5 with the following operation
    // int index = (((long)(object - MEMORY_START) & ((1 << PAGE_SHIFT) - 1)) / allocator->object_size);

    // printf("[object_free] object(page: %d, size: %d, index: %d) to be freed\n", page->page_number, allocator->object_size, index);

    // we are freeing the []
    if (object > page->first_free)
    {
        // XXOX[]O...
        // the object is between the first hole and the second hole
        if (object < (page->start_address + *(int *)page->first_free))
        {
            // printf("[object_free] status 1\n\n");

            *(int *)object = *(int *)page->first_free;
            *(int *)page->first_free = object - page->start_address;
        }
        // XXOOO[]...
        // the object is behind the second hole
        // so we need to iterate over the list to find the last hole in front of the object
        else
        {
            // printf("[object_free] status 2\n\n");

            void *traversal = page->first_free;
            while ((page->start_address + (*(int *)traversal)) < object)
                traversal = page->start_address + (*(int *)traversal);

            *(int *)object = *(int *)traversal;
            *(int *)traversal = object - page->start_address;
        }
    }
    // XX[]XXO...
    // the object is in front of the first hole
    else
    {
        // printf("[object_free] status 3\n\n");
        
        *(int *)object = page->first_free - page->start_address;
        page->first_free = object;
    }
    
    page->object_count--;

    list_crop(&page->list, &page->list);
    // full/partial to partial
    if (page->object_count > 0)
        list_add_tail(&page->list, &allocator->partial);
    // partial to empty
    else
    {
        if (page == allocator->current_page)
            allocator->current_page = NULL;
        block_free(page);
    }

    // if (page != allocator->current_page)
    // {
    //     list_crop(&page->list, &page->list);
    //     // full to partial
    //     if (page->object_count > 0)
    //         list_add_tail(&page->list, &allocator->partial);
    //     // partial to empty
    //     else
    //     {
    //         if (allocator->preserved_empty == NULL)
    //             allocator->preserved_empty = page;
    //         else
    //             block_free(page);
    //     }
    // }

    // printf("[object_free] done\n\n");
}

void *km_allocation(int size)
{
    void *address;

    if (size <= (PAGE_SIZE / 2))
    {
        for (int i = MIN_OBJECT_ORDER; i < MAX_OBJECT_ORDER + 1; i++)
        {
            if (size <= (1 << i))
            {
                address = object_allocation(i - MIN_OBJECT_ORDER);
                // printf("--------------------\n\n");
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
                address = block_allocation(i)->start_address;
                // printf("--------------------\n\n");
                return address;
            }
        }
    }

    printf("[memory_allocation] the requested size is too large!\n");
    return 0;
}

void km_free(void *address)
{
    int page_number = (long)(address - MEMORY_START) >> PAGE_SHIFT;
    struct page *page = &bookkeep[page_number];

    if (page->allocator)
        object_free(address);
    else
        block_free(page);

    // printf("--------------------\n\n");
}

int find_buddy(int page_number, int order)
{
    return page_number ^ (1 << order);
}