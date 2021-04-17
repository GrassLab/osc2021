#ifndef __ALLOCATOR__
#define __ALLOCATOR__

void init_memory();
void allocator();

typedef struct Memblock {
    unsigned long start_addr;
    unsigned long size;
} Memblock;

typedef struct Frame {
    int index;
    int state;
    struct Frame *next_block;
    int chunk_size;
    int chunk_num;
    unsigned char chunks[32];      // ((FRAME_SIZE / pool_sizes[0]) / 8)
    struct Frame *next_chunks;
} Frame;

struct Memory_unit {
    char *name;
    int offset;
} static const memory_unit_list[] = {
    {.name = "B", .offset = 1},
    {.name = "K", .offset = 1024},
    {.name = "M", .offset = 1024 * 1024}
};

#endif