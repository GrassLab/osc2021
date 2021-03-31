#ifndef BUDDY_H
#define BUDDY_H

#include <mini_uart.h>
#include <stdint.h>

struct BuddyInfo {
    // If bits[31] = 1 not allocated and be the master
    // If bits[31] = 0 allocated or be the slave
    // bits[0:30]
    // If it is the master, the power of block size = 4 * (2 ^ (bits[0:30] - 1)) KB, no matter if it is allocated
    // If it is the slave, bits[0:30] = 0
    uint32_t status;
    //  If status is not equal to -1, it point to the next same size block.
    int next;
};

template<uint32_t SIZE_POW>
class Buddy {
public:
    const uint32_t STATUS_NOT_ALLOCATED = 1 << 31;
    const uint32_t STATUS_POW_SIZE = ~STATUS_NOT_ALLOCATED;
    Buddy() {
        for (uint32_t i = 0; i < (1 << SIZE_POW); i++) {
            info[i] = {0, -1};
        }
        for (uint32_t i = 0; i <= SIZE_POW; i++) {
            first_elem[i] = -1;
        }
        info[0] = {((1 << 31) | (SIZE_POW + 1)), -1};
        first_elem[SIZE_POW] = 0;
    }
    // return the offset of the allocated memory
    uint32_t Allocate(const uint32_t alloc_size_pow) {
        int result = AllocateInternal(alloc_size_pow);
        IO() << "Allocate buddy " << result << " with size 2 ^ " << alloc_size_pow << "\r\n";
        return result;
    }
    void Free(uint32_t offset) {
        if (!(info[offset].status & STATUS_NOT_ALLOCATED)) {
            uint32_t alloc_size_pow_plus_1 = info[offset].status;
            if (alloc_size_pow_plus_1 == 0) return;
            uint32_t alloc_size_pow = alloc_size_pow_plus_1 - 1;
            int find_buddy_flag = 1 << alloc_size_pow;
            info[offset].status |= STATUS_NOT_ALLOCATED;
            PutIntoList(offset, alloc_size_pow);
            CombineBuddy(offset, alloc_size_pow);
            IO() << "Freed buddy " << offset << "\r\n";
        }
    }
private:
    void CombineBuddy(uint32_t offset, uint32_t alloc_size_pow) {
        int find_buddy_flag = 1 << alloc_size_pow;
        uint32_t other_offset = offset ^ find_buddy_flag;
        if (other_offset < (1 << SIZE_POW) &&
            (info[other_offset].status & STATUS_NOT_ALLOCATED) &&
            (info[offset].status & STATUS_POW_SIZE) == (info[other_offset].status & STATUS_POW_SIZE)) {
            // Remove both offset from the linked list
            RemoveFromList(offset, alloc_size_pow);
            RemoveFromList(other_offset, alloc_size_pow);
            info[other_offset].next = -1;
            // Combine 2 of them
            int first_offset = offset < other_offset ? offset : other_offset;
            int second_offset = offset < other_offset ? other_offset : offset;
            info[second_offset].status = 0;
            info[second_offset].next = -1;
            alloc_size_pow++;
            info[first_offset].status = STATUS_NOT_ALLOCATED | (alloc_size_pow + 1);
            info[first_offset].next = -1;
            IO() << "Combine buddy " << first_offset << " with buddy " << second_offset << "\r\n";
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
    }
    void PutIntoList(uint32_t offset, uint32_t alloc_size_pow) {
        int iter_offset = first_elem[alloc_size_pow];
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
        int result;
        if (hasSameSizeElem) {
            result = first_elem[alloc_size_pow];
            first_elem[alloc_size_pow] = info[result].next;
        }
        else {
            result = Allocate(alloc_size_pow + 1);
        }
        info[result].status = alloc_size_pow + 1;
        info[result].next = -1;
        if (!hasSameSizeElem) {
            int bottom_half_offset = result + (1 << alloc_size_pow);
            info[bottom_half_offset].status = (alloc_size_pow + 1) | STATUS_NOT_ALLOCATED;
            info[bottom_half_offset].next = -1;
            first_elem[alloc_size_pow] = bottom_half_offset;
            IO() << "Split buddy " << result << " with size 2^" << (alloc_size_pow + 1) << " into 2 buddy with size 2^" << alloc_size_pow << ":" << result << ", " << bottom_half_offset << "\r\n";
        }
        return result;
    }
    BuddyInfo info[(1 << SIZE_POW)];
    int first_elem[SIZE_POW + 1];
};

#endif
