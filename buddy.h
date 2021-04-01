#ifndef BUDDY_H
#define BUDDY_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_BSIZE 8

struct status_node_t {
    struct status_node_t* prev;
    struct status_node_t* next;
    struct buddy_node_t* buddy_node;
    uint64_t val;
};

struct chunk_node_t {
    struct buddy_node_t* buddy_node;
    struct chunk_node_t* next;
    uint64_t chunk_size; // minimum size 32bit
    uint64_t index;
};

struct buddy_node_t {
    struct buddy_node_t* left;
    struct buddy_node_t* right;
    struct buddy_node_t* parent;
    struct status_node_t status;
    struct chunk_node_t chunk[128];
    uint64_t addr;
    uint64_t index;
    uint64_t size;
    bool free;
    bool split;
};

extern struct buddy_node_t buddy_node_pool[1024];

void buddy_init();
struct buddy_node_t* new_buddy(uint64_t addr, uint64_t size);
void free_node_push(struct status_node_t* node);
struct status_node_t* free_node_pop(uint64_t size);
void free_node_delete(struct status_node_t* node);
struct buddy_node_t* buddy_system(uint64_t size);
struct buddy_node_t* buddy_alloc(uint64_t size);
void buddy_free(struct buddy_node_t* node);

#endif