#ifndef MEMALLOC_H
#define MEMALLOC_H
#include <buddy.h>

class MemAlloc {
    Buddy<12> buddy;
    char* allocated_memory[1024] = {nullptr};
    char* base = (char*)0x10000000;
    short left_size = 0;
    char* current_ptr = 0;

public:
    char* malloc(uint32_t size);
    bool free(char* ptr);
};

#endif
