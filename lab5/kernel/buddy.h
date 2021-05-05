#ifndef BUDDY_H
#define BUDDY_H

#include "types.h"

template<uint32_t SIZE_POW>
struct BuddyInfo {
    // If allocated = 1 allocated or be the slave
    // If allocated = 0 not allocated and be the master
    char allocated: 1;
    // If it is the master, the power of block size = 4 * (2 ^ (size_pow_plus_1 - 1)) KB, no matter if it is allocated
    // If it is the slave, size_pow_plus_1 = 0
    char size_pow_plus_1: 6;
    //  If status is not equal to -1, it point to the next same size block.
    int32_t next: SIZE_POW + 1;
};

template<uint32_t SIZE_POW>
class Buddy {
public:
    void Init() {
        for (uint32_t i = 0; i < (1 << SIZE_POW); i++) {
            info[i] = {1, 0, -1};
        }
        for (uint32_t i = 0; i <= SIZE_POW; i++) {
            first_elem[i] = -1;
        }
        info[0] = {0, SIZE_POW + 1, -1};
        first_elem[SIZE_POW] = 0;
    }

    // return the offset of the allocated memory
    uint32_t Allocate(const uint32_t alloc_size_pow) {
        int32_t result = AllocateInternal(alloc_size_pow);
        // IO() << "Allocate buddy " << result << " with size " << (1<<alloc_size_pow) << "\r\n";
        return result;
    }
    void Free(uint32_t offset) {
        if (info[offset].allocated && info[offset].size_pow_plus_1 > 0) {
            uint32_t alloc_size_pow = info[offset].size_pow_plus_1 - 1;
            int32_t find_buddy_flag = 1 << alloc_size_pow;
            info[offset].allocated = 0;
            PutIntoList(offset, alloc_size_pow);
            CombineBuddy(offset, alloc_size_pow);
            // IO() << "Freed buddy " << offset << "\r\n";
        }
        else {
            // IO() << "Memory not allocated\r\n";
        }
    }
private:
    void CombineBuddy(uint32_t offset, uint32_t alloc_size_pow) {
        int32_t find_buddy_flag = 1 << alloc_size_pow;
        uint32_t other_offset = offset ^ find_buddy_flag;
        if (other_offset < (1 << SIZE_POW) && !info[other_offset].allocated && info[offset].size_pow_plus_1 == info[other_offset].size_pow_plus_1) {
            // Remove both offset from the linked list
            RemoveFromList(offset, alloc_size_pow);
            RemoveFromList(other_offset, alloc_size_pow);
            // Combine 2 of them
            int32_t first_offset = offset < other_offset ? offset : other_offset;
            int32_t second_offset = offset < other_offset ? other_offset : offset;
            info[second_offset].allocated = 1; // Slave
            info[second_offset].size_pow_plus_1 = 0;
            info[second_offset].next = -1;
            alloc_size_pow++;
            info[first_offset].allocated = 0;
            info[first_offset].size_pow_plus_1 = alloc_size_pow + 1;
            info[first_offset].next = -1;
            // IO() << "Combine buddy " << first_offset << " with buddy " << second_offset << "\r\n";
            // Put info[first_offset] into linked list
            PutIntoList(first_offset, alloc_size_pow);
            CombineBuddy(first_offset, alloc_size_pow);
        }
    }
    void RemoveFromList(uint32_t offset, uint32_t alloc_size_pow) {
        int iter_offset = first_elem[alloc_size_pow];
        if (iter_offset == offset) {
            first_elem[alloc_size_pow] = info[offset].next;
        }
        else {
            while (info[iter_offset].next != offset) {
                iter_offset = info[iter_offset].next;
            }
            info[iter_offset].next = info[offset].next;
        }
        info[offset].next = -1;
    }
    void PutIntoList(uint32_t offset, uint32_t alloc_size_pow) {
        int32_t iter_offset = first_elem[alloc_size_pow];
        if (iter_offset == -1) {
            first_elem[alloc_size_pow] = offset;
        }
        else if (iter_offset > offset) {
            info[offset].next = first_elem[alloc_size_pow];
            first_elem[alloc_size_pow] = offset;
        }
        else {
            while (info[iter_offset].next < offset) {
                iter_offset = info[iter_offset].next;
            }
            info[offset].next = info[iter_offset].next;
            info[iter_offset].next = offset;
        }
    }
    // return the offset of the allocated memory
    uint32_t AllocateInternal(const uint32_t alloc_size_pow) {
        bool hasSameSizeElem = first_elem[alloc_size_pow] != -1;
        int32_t result;
        if (hasSameSizeElem) {
            result = first_elem[alloc_size_pow];
            first_elem[alloc_size_pow] = info[result].next;
        }
        else {
            result = AllocateInternal(alloc_size_pow + 1);
        }
        info[result].allocated = 1;
        info[result].size_pow_plus_1 = alloc_size_pow + 1;
        info[result].next = -1;
        if (!hasSameSizeElem) {
            int32_t bottom_half_offset = result + (1 << alloc_size_pow);
            info[bottom_half_offset].allocated = 0;
            info[bottom_half_offset].size_pow_plus_1 = alloc_size_pow + 1;
            info[bottom_half_offset].next = -1;
            first_elem[alloc_size_pow] = bottom_half_offset;
            // IO() << "Split buddy " << result << " with size " << (1<<(alloc_size_pow + 1)) << " into 2 buddy with size " << (1<<alloc_size_pow) << ": " << result << ", " << bottom_half_offset << "\r\n";
        }
        return result;
    }
    BuddyInfo<SIZE_POW> info[(1 << SIZE_POW)];
    int32_t first_elem[SIZE_POW + 1];
};

#endif
