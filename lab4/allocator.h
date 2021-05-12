#ifndef __ALLOCATOR__
#define __ALLOCATOR__

#define FRAME_SIZE          4096    // 4K

typedef struct Memblock {
    unsigned long start_addr;
    unsigned long size;
    unsigned long record_addr;  // record frame_array start address in RECORD_DYNAMIC_BLOCK table
} Memblock;

/*
 * state >= 0 : There is an allocable, contiguous memory that starts from the idx’th frame with size=2^(val) × 4kb. 
 * state = -1 : The idx’th frame is free, but it belongs to a larger contiguous memory block.
 *              Hence, buddy system doesn’t directly allocate it.
 * state = -2 : The idx’th frame is already allocated, hence not allocable.
 * state = -3 : no frame to use
 */
typedef struct Frame {
    int index;
    int state;
    int dynamic_block_index;
    struct Frame *next_block;
    int chunk_size;
    unsigned char chunks[32];      // ((FRAME_SIZE / pool_sizes[0]) / 8)
    struct Frame *next_chunks;
} Frame;

void init_memory();
unsigned long get_memory(unsigned long size);
void free_memory(unsigned long size, unsigned long address);

#endif