#include <memalloc.h>

char* MemAlloc::malloc(uint32_t size) {
    if (size <= 0) {
        IO() << "size should be greater than 0\r\n";
        return nullptr;
    }
    size = (size + 3) & ~3; // Align 4
    char* result;
    if (current_ptr != nullptr && size <= left_size) {
        result = current_ptr;
        current_ptr = current_ptr + size;
        left_size -= size;
    }
    else if (size <= 4096) {
        uint32_t size_pow = 0;
        result = base + (buddy.Allocate(0) << 12);
        left_size = 4096 - size;
        current_ptr = result + size;
    }
    else {
        size = (size + 4095) & ~4095; // Align 4KB
        int size_pow = 0;
        while ((1 << (12 + size_pow)) < size) {
            size_pow++;
        }
        result = base + (buddy.Allocate(size_pow) << 12);
        current_ptr = nullptr;
        left_size = 0;
    }
    for (int i = 0; i < 1024; i++) {
        if (allocated_memory[i] == nullptr) {
            allocated_memory[i] = result;
            break;
        }
    }
    return result;
}

static inline bool sameChunk(char* ptr1, char* ptr2) {
    uint64_t a = uint64_t(ptr1) & ~(0x1000 - 1);
    uint64_t b = uint64_t(ptr2) & ~(0x1000 - 1);
    return a == b;
}

bool MemAlloc::free(char* ptr) {
    if (ptr == nullptr) {
        IO() << "ptr should not be 0\r\n";
        return false;
    }
    bool hasSameChunk = false;
    for (int i = 0; i < 1024; i++) {
        if (allocated_memory[i] == ptr) {
            allocated_memory[i] = nullptr;
        }
        else if (sameChunk(ptr, allocated_memory[i])) {
            hasSameChunk = true;
        }
    }
    if (!hasSameChunk) {
        if (sameChunk(ptr, current_ptr)) {
            current_ptr = nullptr;
        }
        buddy.Free((ptr - base) >> 12);
    }
    return true;
}
