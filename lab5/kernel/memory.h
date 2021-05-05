#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "buddy.h"

class MemAlloc {
    Buddy<12> buddy;
    uint64_t allocated_memory_count;
    char** allocated_memory;
    char* base;
    short left_size;
    char* current_ptr;

public:
    void Init();
    char* malloc(uint32_t size);
    bool free(char* ptr);
};

void* malloc(uint32_t size);
bool free(void* ptr);

#endif
