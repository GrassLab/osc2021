#ifndef DMALLOC_H
#define DMALLOC_H

#include "buddy.h"
#include <stdint.h>

#define MAX_CSIZE 7 // 32 64 128 .... 2048

void free_chunk_push(struct chunk_node_t* node, uint64_t size);
struct chunk_node_t* free_chunk_pop(uint64_t size);
struct chunk_node_t* dynamic_allocate(uint64_t size);
void chunk_free(struct chunk_node_t* node);

#endif