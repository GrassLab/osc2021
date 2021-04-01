#include "dmalloc.h"
#include "buddy.h"
#include "uart.h"

struct chunk_node_t* free_chunck_entry[MAX_CSIZE];

void free_chunk_push(struct chunk_node_t* node, uint64_t size)
{
    node->next = free_chunck_entry[size];
    free_chunck_entry[size] = node;
}

struct chunk_node_t* free_chunk_pop(uint64_t size)
{
    struct chunk_node_t* node = free_chunck_entry[size];
    free_chunck_entry[size] = node->next;
    return node;
}

struct chunk_node_t* dynamic_allocate(uint64_t size)
{
    print_string("Proper block size: ");
    print_integer(1 << (size + 5));
    print_string("B\n");
    if (!free_chunck_entry[size]) {
        print_string("Current size of chunk is empty, allocating...\n");
        uint64_t chunk_size = 1 << (size + 5);
        struct buddy_node_t* node = buddy_system(0);
        node->free = false;
        node->split = true;
        node->chunk[0].chunk_size = chunk_size;
        node->chunk[0].index = 0;
        node->chunk[0].buddy_node = node;
        int cnt = 4096 / chunk_size;
        for (int i = 1; i < cnt; i++) {
            node->chunk[i].chunk_size = chunk_size;
            node->chunk[i].index = cnt - i;
            node->chunk[i].buddy_node = node;
            free_chunk_push(&node->chunk[i], size);
        }
        return &node->chunk[0];
    } else {
        struct chunk_node_t* chunk_node = free_chunk_pop(size);
        print_string("Allocation successful\n");
        print_string("Address: ");
        print_address(chunk_node->buddy_node->addr + chunk_node->chunk_size * chunk_node->index);
        print_string("\n");
        return chunk_node;
    }
}

void chunk_free(struct chunk_node_t* node)
{
    print_string("Start to free chunk\n");
    print_string("Current size of chunk: ");
    print_integer(node->chunk_size);
    print_string("B\n");
    uint64_t size = __builtin_ctz(node->chunk_size / 32);
    free_chunk_push(node, size);
    print_string("Successfully free\n");
}