#pragma once

#include <types.h>
#include <memory_addr.h>
#include <memory_func.h>
#include <mini_uart.h>

#define BUDDY_MAX_SIZE_POW 12
#define BUDDY_INFO_COUNT (1 << BUDDY_MAX_SIZE_POW)
#define BUDDY_INFO_SIZE (BUDDY_INFO_COUNT * sizeof(BuddyInfo))

struct BuddyInfo {
    // 1: allocated and be the master
    // 0: not allocated or be the slave
    bool allocated: 1;
    // 1: master 
    // 0: slave
    bool is_master: 1;
    // If master: indicated the Power of the size
    // If slave: Not defined
    char size_pow: 6;

    bool has_prev: 1;
    bool has_next: 1;

    uint32_t prev: BUDDY_MAX_SIZE_POW;
    //  If status is not equal to -1, it point to the next same size block.
    uint32_t next: BUDDY_MAX_SIZE_POW;
};

class Buddy {
public:
    void Init();

    // return the offset of the allocated memory
    uint32_t Allocate(const uint32_t alloc_size_pow);
    void Free(uint32_t offset);
private:
    void CombineBuddy(uint32_t offset);
    void RemoveFromList(uint32_t offset);
    void PutIntoList(uint32_t offset);
    // return the offset of the allocated memory
    uint32_t AllocateInternal(const uint32_t alloc_size_pow);
    bool has_first[BUDDY_MAX_SIZE_POW + 1];
    int32_t first_elems[BUDDY_MAX_SIZE_POW + 1];
    BuddyInfo *info;
};
