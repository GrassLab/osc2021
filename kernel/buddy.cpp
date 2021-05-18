#include <kernel/buddy.h>

void Buddy::Init() {
    static_assert(BUDDY_INFO_SIZE <= 0x10000, "Buddy size is not large enough");
    info = (BuddyInfo*)BUDDY_INFO_BASE;
    memset(info, 0, BUDDY_INFO_SIZE);
    memset(first_elems, 0, sizeof(first_elems));
    memset(has_first, 0, sizeof(has_first));
    info[0] = {0, true, BUDDY_MAX_SIZE_POW, false, false, 0, 0};
    has_first[BUDDY_MAX_SIZE_POW] = true;
    first_elems[BUDDY_MAX_SIZE_POW] = 0;
}
uint32_t Buddy::Allocate(const uint32_t alloc_size_pow) {
    int32_t result = AllocateInternal(alloc_size_pow);
    return result;
}
void Buddy::Free(uint32_t offset) {
    if (info[offset].allocated && info[offset].is_master) {
        uint32_t alloc_size_pow = info[offset].size_pow;
        info[offset].allocated = 0;
        PutIntoList(offset);
        CombineBuddy(offset);
    }
}
void Buddy::CombineBuddy(uint32_t offset) {
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
void Buddy::RemoveFromList(uint32_t offset) {
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
void Buddy::PutIntoList(uint32_t offset) {
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
uint32_t Buddy::AllocateInternal(const uint32_t alloc_size_pow) {
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
