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

	char output_buffer[30] = { 0 };
	uart_puts("[debug] required order: ");
	itoa(required_order, output_buffer, 10);
	uart_puts(output_buffer);
	uart_puts("\n");

	/* find allocatable order in frame_freelist */
	while(frame_freelist[allocatable_order] == NULL)
		allocatable_order++;

	/* splitting higher order block for appropriate size */
	while(allocatable_order != required_order) {

        for(int i = 0; i < 30; i++)
            output_buffer[i] = 0;
        itoa(allocatable_order, output_buffer, 10);
        uart_puts("[debug] allocatable order: ");
        uart_puts(output_buffer);
        uart_puts("\n");
		
		int allocatable_index = frame_freelist[allocatable_order]->index;

        for(int i = 0; i < 30; i++)
            output_buffer[i] = 0;
        itoa(allocatable_index, output_buffer, 10);
        uart_puts("[debug] allocatable index: ");
        uart_puts(output_buffer);
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

	for(int i = 0; i < pow(2, the_frame_array[required_index].order); i++)
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

void free_frame(void *adr) {
    uart_puts("Hello");
}
