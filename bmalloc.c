#include "bmalloc.h"
#include "buddy.h"
#include "dmalloc.h"
#include "uart.h"

void* bmalloc(uint64_t size)
{
    print_string("Malloc size: ");
    print_integer(size);
    print_string("B\n");
    void* addr = 0;
    if (size >= 4096) {
        print_string("Buddy system\n");
        uint64_t allocate_size = 1 << 12;
        uint64_t val = 0;
        while (size > allocate_size) {
            allocate_size <<= 1;
            val++;
        }
        struct buddy_node_t* node = buddy_alloc(val);
        addr = (void*)node->index;
        return addr;
    } else {
        print_string("Dynamic allocation\n");
        uint64_t allocate_size = 32;
        uint64_t val = 0;
        while (size > allocate_size) {
            allocate_size <<= 1;
            val++;
        }
        struct chunk_node_t* node = dynamic_allocate(val);
        addr = (void*)(node->buddy_node->index + node->index * 2048);
        return addr;
    }
}

void bfree(void* addr)
{
    print_string("Free memory\n");
    print_string("Memory address: ");
    uint64_t buddy_index = (uint64_t)addr % 2048;
    uint64_t chunk_index = (uint64_t)addr / 2048;
    struct buddy_node_t* node = &buddy_node_pool[buddy_index];
    if (!node->split) {
        print_address(node->addr);
        print_string("\n");
        buddy_free(node);
    } else {
        print_address(node->addr + node->chunk[chunk_index].chunk_size * chunk_index);
        print_string("\n");
        chunk_free(&node->chunk[chunk_index]);
    }
}