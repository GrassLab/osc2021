#include "buddy.h"
#include "type.h"
#include "math.h"
#include "uart.h"
#include "util.h"

/* 
 * This array represents the allocation status of the memory.
 * The size of allocable memory is 256MB (0x1000_0000 ~ 0x2000_0000) and frame size is 4KB,
 * then, this array would consist 65536 entries.
 */
struct buddy_frame the_frame_array[FRAME_NUMBERS];

/* 
 * This array represents the availability of various size contiguous memory block size.
 * The maximum block size is 2^16 * 4 KB = 256MB.
 */
struct buddy_frame *frame_freelist[FRAME_MAX_ORDER];


void buddy_init() {
    uart_puts("[debug] Memory allocation initializing ...\n");

    for(int i = 0; i < FRAME_NUMBERS; i++) {
        the_frame_array[i].index          =   i;
        the_frame_array[i].order          =   FREE_FRAME_ALLOCATABLE;
        the_frame_array[i].used_order     =   -1;
        the_frame_array[i].start_address  =   BASE_ADDRESS + FRAME_SIZE * i;
        the_frame_array[i].next           =   NULL;
    }

    for(int i = 0; i < FRAME_MAX_ORDER; i++)
        frame_freelist[i] = NULL;

    frame_freelist[FRAME_MAX_ORDER - 1] = &the_frame_array[0];

    the_frame_array[0].order = FRAME_MAX_ORDER - 1;

    uart_puts("[debug] Finish !\n");
}

void *allocate_frame(int required_size_in_kbyte) {
	int required_order = ceil(log(2, (float)required_size_in_kbyte / 4));
	int allocatable_order = required_order;

	uart_puts("[debug] required order: ");
    uart_puti(required_order, 10);
	uart_puts("\n");

	/* find allocatable order in frame_freelist */
	while(frame_freelist[allocatable_order] == NULL) {
        allocatable_order++;
        if(allocatable_order == FRAME_MAX_ORDER)
            return NULL;
    }

	/* splitting higher order block for appropriate size */
	while(allocatable_order != required_order) {

        uart_puts("[debug] allocatable order: ");
        uart_puti(allocatable_order, 10);
        uart_puts("\n");
		
		int allocatable_index = frame_freelist[allocatable_order]->index;

        uart_puts("[debug] allocatable index: ");
        uart_puti(allocatable_index, 10);
        uart_puts("\n");

		/* updating the_frame_array */
		the_frame_array[allocatable_index].order -= 1;

		int splitted_index = the_frame_array[allocatable_index].index + pow(2, the_frame_array[allocatable_index].order);

		the_frame_array[splitted_index].order = the_frame_array[allocatable_index].order;

		/* updating frame_freelist */
		struct buddy_frame *a_tmp = &the_frame_array[allocatable_index];
		struct buddy_frame *b_tmp = &the_frame_array[splitted_index];

		freelist_deletion(allocatable_order, a_tmp);

		freelist_insertion(a_tmp->order, a_tmp);
		freelist_insertion(b_tmp->order, b_tmp);

		allocatable_order -= 1;
	}

	int required_index = frame_freelist[required_order]->index;
	freelist_deletion(required_order, frame_freelist[required_order]);

    the_frame_array[required_index].used_order = required_order;

	for(int i = 0; i < pow(2, required_order); i++)
        the_frame_array[required_index + i].order = USED_FRAME_UNALLOCATABLE;

	return &the_frame_array[required_index];
}

void freelist_deletion(int order, struct buddy_frame *frame) {
	struct buddy_frame *current = frame_freelist[order];
    if(current == frame) {
        if(current->next == NULL)
            frame_freelist[order] = NULL;
        else
            frame_freelist[order] = current->next;
        return ;
    }

	while(current->next != frame) {
        current = current->next;
    }

	current->next = frame->next;
	frame->next = NULL;
	
}

void freelist_insertion(int order, struct buddy_frame *frame) {
	struct buddy_frame *current = frame_freelist[order];
    if(current == NULL) {
        frame_freelist[order] = frame;
        frame->next = NULL;
        return ;
    }

	while(current->next != NULL) {
        current = current->next;
    }

	current->next = frame;
	frame->next = NULL;
}

int free_frame_by_size(int freed_size_in_kbyte) {
    /* size need to be 2^ord * 4 */
    if(freed_size_in_kbyte % 4 != 0) 
        return -1;
    
    int freed_frames = freed_size_in_kbyte / 4;

    // int freed_order = ceil(log(2, freed_frame));
    int freed_order = log(2, freed_frames);

    int i;
    for(i = 0; i < FRAME_NUMBERS; i++) {
        if(the_frame_array[i].used_order == freed_order)
            break;
        if(i == FRAME_NUMBERS - 1)
            return -1;
    }
    
    int freeable_index = i;

    uart_puts("[debug] freeable_index: ");
    uart_puti(freeable_index, 10);
    uart_puts("\n");

    /* updating the_frame_array */
    for(i = 0; i < freed_frames; i++) 
        the_frame_array[freeable_index + i].order = FREE_FRAME_ALLOCATABLE;
    the_frame_array[freeable_index].order = freed_order;
    the_frame_array[freeable_index].used_order = -1;

    /* updating frame_freelist */
    freelist_insertion(freed_order, &the_frame_array[freeable_index]);

    /* Find buddy and merge iteratively */
    int buddy_index = freeable_index ^ freed_frames;

    /*     0    1    2    3    4    5    6    7    8    9   10   
     *  |----|----|----|----|----|----|----|----|----|----|----|----|
     *  |  F |    |  s |    |    |    |    |    |    |    |    | ...|    
     *  |----|----|----|----|----|----|----|----|----|----|----|----|
     */

    int first, second;
    
    if(freeable_index > buddy_index) {
    	first = buddy_index;
    	second = freeable_index;
    } else {
    	first = freeable_index;
    	second = buddy_index;
    }

    while (the_frame_array[first].order >= 0 && the_frame_array[second].order >= 0) {        
        uart_puts("[debug] Merge ");
        uart_puti(first, 10);
        uart_puts(" with ");
        uart_puti(second, 10);
        uart_puts("\n");


        /* updating frame_freelist */
        freelist_deletion(the_frame_array[first].order, &the_frame_array[first]);
		freelist_deletion(the_frame_array[second].order, &the_frame_array[second]);

		freelist_insertion(the_frame_array[first].order + 1, &the_frame_array[first]);
        /* updating the_frame_array */
        the_frame_array[first].order += 1;
		the_frame_array[second].order = FREE_FRAME_ALLOCATABLE;

        int next_index = first ^ pow(2, the_frame_array[first].order);
    	if(next_index > second)
		    second = next_index;
	    else {
            second = first;
            first = next_index;
        }

        if(second == FRAME_NUMBERS)
            break;
    }

    return first;
}

int free_frame_by_index(int freed_index) {
    return 1;
}



void print_available_memory_with_uart() {
    for(int i = 0; i < FRAME_NUMBERS;) {
        if(the_frame_array[i].order >= 0) {            
            uart_puts("Frame index: ");
            uart_puti(i, 10);
            uart_puts(", start address: 0x");
            uart_puti(the_frame_array[i].start_address, 16);
            uart_puts(", size: ");
            uart_puti(pow(2, the_frame_array[i].order) * 4, 10);
            uart_puts(" KB\n");

            i += pow(2, the_frame_array[i].order) - 1;
        }
        i++;
    }
}