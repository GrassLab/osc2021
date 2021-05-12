#pragma once

#include <types.h>
#include <kernel/buddy.h>

class MemAlloc {
    Buddy buddy;
    uint64_t allocated_memory_count;
    char** allocated_memory;
    short left_size;
    char* current_ptr;

public:
    void Init();
    char* malloc(uint32_t size);
    bool free(char* ptr);
};

void* malloc(uint32_t size);
bool free(void* ptr);
