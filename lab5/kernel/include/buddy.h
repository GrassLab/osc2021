#pragma once

#include <types.h>
#include <memory_addr.h>
#include <memory_func.h>

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

    uint64_t prev: BUDDY_MAX_SIZE_POW;
    //  If status is not equal to -1, it point to the next same size block.
    uint64_t next: BUDDY_MAX_SIZE_POW;
};

class Buddy {
public:
    void Init() {
        static_assert(BUDDY_INFO_SIZE <= 0x10000, "Buddy size is not large enough");
        memset((void *)BUDDY_INFO_BASE, 0, BUDDY_INFO_SIZE);
        memset(&first_elems, 0, sizeof(first_elems));
        memset(&has_first, 0, sizeof(has_first));
        info[0] = {0, true, BUDDY_MAX_SIZE_POW, false, false, 0, 0};
        has_first[BUDDY_MAX_SIZE_POW] = true;
        first_elems[BUDDY_MAX_SIZE_POW] = 0;
        info = (BuddyInfo*)BUDDY_INFO_BASE;
    }

    // return the offset of the allocated memory
    uint32_t Allocate(const uint32_t alloc_size_pow) {
        int32_t result = AllocateInternal(alloc_size_pow);
        return result;
    }
    void Free(uint32_t offset) {
        if (info[offset].allocated && info[offset].is_master) {
            uint32_t alloc_size_pow = info[offset].size_pow;
            info[offset].allocated = 0;
            PutIntoList(offset);
            CombineBuddy(offset);
        }
    }
private:
    void CombineBuddy(uint32_t offset) {
        uint32_t alloc_size_pow = info[offset].size_pow;
        int32_t find_buddy_flag = 1 << alloc_size_pow;
        uint32_t other_offset = offset ^ find_buddy_flag;
        BuddyInfo& current = info[offset];
        BuddyInfo& other = info[other_offset];
        if (other_offset < (1 << BUDDY_MAX_SIZE_POW) && other.is_master && !other.allocated && current.size_pow == other.size_pow) {
            // Remove both offset from the linked list
            RemoveFromList(offset);
            RemoveFromList(other_offset);
            // Combine 2 of them
            int32_t first_offset = offset < other_offset ? offset : other_offset;
            int32_t second_offset = offset < other_offset ? other_offset : offset;
            info[second_offset].is_master = false;
            info[first_offset].size_pow = alloc_size_pow + 1;
            PutIntoList(first_offset);
            CombineBuddy(first_offset);
        }
    }
    void RemoveFromList(uint32_t offset) {
        uint32_t alloc_size_pow = info[offset].size_pow;
        if (first_elems[alloc_size_pow] == offset) {
            has_first[alloc_size_pow] = info[offset].has_next;
            if (info[offset].has_next) {
                first_elems[alloc_size_pow] = info[offset].next;
                info[info[offset].next].has_prev = false;
            }
        }
        else {
            info[info[offset].prev].has_next = info[offset].has_next;
            if (info[offset].has_next) {
                info[info[offset].prev].next = info[offset].next;
                info[info[offset].next].has_prev = true;
                info[info[offset].next].prev = info[offset].prev;
            }
        }
    }
    void PutIntoList(uint32_t offset) {
        uint32_t alloc_size_pow = info[offset].size_pow;
        if (!has_first[alloc_size_pow]) {
            has_first[alloc_size_pow] = true;
            first_elems[alloc_size_pow] = offset;
            info[offset].has_prev = false;
            info[offset].has_next = false;
        }
        else {
            info[offset].has_next = true;
            info[offset].next = first_elems[alloc_size_pow];
            info[offset].has_prev = false;
            info[first_elems[alloc_size_pow]].has_prev = true;
            info[first_elems[alloc_size_pow]].prev = offset;
            first_elems[alloc_size_pow] = offset;
        }
    }
    // return the offset of the allocated memory
    uint32_t AllocateInternal(const uint32_t alloc_size_pow) {
        int32_t result;
        if (has_first[alloc_size_pow]) {
            result = first_elems[alloc_size_pow];
            RemoveFromList(result);
        }
        else {
            result = AllocateInternal(alloc_size_pow + 1);
            int32_t bottom_half_offset = result + (1 << alloc_size_pow);
            info[bottom_half_offset].allocated = false;
            info[bottom_half_offset].is_master = true;
            info[bottom_half_offset].size_pow = alloc_size_pow;
            info[result].size_pow = alloc_size_pow;
            PutIntoList(bottom_half_offset);
        }
        info[result].allocated = true;
        return result;
    }
    bool has_first[BUDDY_MAX_SIZE_POW + 1];
    int32_t first_elems[BUDDY_MAX_SIZE_POW + 1];
    BuddyInfo *info;
};
