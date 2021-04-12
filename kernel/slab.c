#include "slab.h"
#include "math.h"
#include "uart.h"

struct cache_t slab_allocator[4];
int frames_belong_to_cache[FRAME_NUMBERS];
int allocated_object_in_frame[FRAME_NUMBERS];

void slab_init() {
	for(int i = 0; i < 4; i++) {
		slab_allocator[i].size = 16 * pow(2, i);
		slab_allocator[i].free_head = NULL;
	}

	for(int i = 0; i < FRAME_NUMBERS; i++) {
		allocated_object_in_frame[i] = 0;
		frames_belong_to_cache[i] = -1;
	}
}

static unsigned int roundup_size(unsigned int size) {
	if(size <= 16)
		return 16;
	else if(size > 16 && size <= 32)
		return 32;
	else if(size > 32 && size <= 64)
		return 64;
	else if(size > 64 && size <= 128)
		return 128;
	else
		return 0;
}

static int required_size_to_cache_index(unsigned int size) {
	return log(2, size / 16);
}

static int address_to_frame_array_index(uint64_t address) {
	return ceil((address - BASE_ADDRESS) / FRAME_SIZE);
}

static int init_allocated_frame(uint64_t address, unsigned int object_size) {
	for(int i = 0; i < (FRAME_SIZE / object_size); i++) {
        uint64_t *t = (uint64_t *)address + (i * object_size);
        *t = address + ((i + 1) * object_size);
    }

    uint64_t *t = (uint64_t *)address + FRAME_SIZE - object_size;
    *t = 0;

    return 0;
}

static int address_to_cache_index(uint64_t address) {
	int frame_array_index = address_to_frame_array_index(address);
	return frames_belong_to_cache[frame_array_index];
}

uint64_t *kmalloc(unsigned int size) {
	size = roundup_size(size);

    uart_puts("[debug] kmalloc size: 0x");
    uart_puti(size, 16);
    uart_puts("\n");

	if(size == 0)
		return NULL;

	int cache_index = required_size_to_cache_index(size);

	if(slab_allocator[cache_index].free_head == NULL) {
        uart_puts("[debug] allocating space with new frame for kmalloc.\n");

		// Need to allocate
        struct buddy_frame *frame = allocate_frame(4);
		uint64_t frame_adr = frame->start_address;


		if(init_allocated_frame(frame_adr, size) == 0) {
			// Updating freelist
			uint64_t adr =  frame_adr;

            slab_allocator[cache_index].free_head = (uint64_t *)(frame_adr + size);

			// Updating allocated_object_in_frame array
			int frame_array_index = address_to_frame_array_index(adr);
			allocated_object_in_frame[frame_array_index]++;

			// Updateing frames_belong_to_cache array
			frames_belong_to_cache[frame_array_index] = cache_index;

			return (uint64_t *)adr;
		} else
			return NULL;
	} else {
        uart_puts("[debug] allocating space with exist frame for kmalloc.\n");

		// Updating freelist
		uint64_t adr = (uint64_t)slab_allocator[cache_index].free_head;
        slab_allocator[cache_index].free_head = (uint64_t *)(adr + size);

		// Updating allocated_object_in_frame array
		int frame_array_index = address_to_frame_array_index(adr);
		allocated_object_in_frame[frame_array_index]++;

		return (uint64_t *)adr;
	}
}

int kfree(uint64_t address) {
	int cache_index = address_to_cache_index(address);

    uint64_t tmp = *(slab_allocator[cache_index].free_head);
	uint64_t *t = slab_allocator[cache_index].free_head;
	*t = address;
	address = tmp;

	// Updating allocated_object_in_frame array
	int frame_array_index = address_to_frame_array_index(address);
	allocated_object_in_frame[frame_array_index]--;

	if(allocated_object_in_frame[frame_array_index] == 0) {
		frames_belong_to_cache[frame_array_index] = -1;
		return free_frame_by_index(frame_array_index);
	}

	return 0;
}
