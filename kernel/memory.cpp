#include <memory.h>
#include <types.h>
#include <memory_addr.h>
#include <memory_func.h>
#include <mini_uart.h>

char* MemAlloc::malloc(uint32_t size) {
    if (size <= 0) {
        // IO() << "size should be greater than 0\r\n";
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
        result = (char*)MOMORY_ALLOC_BASE + (buddy.Allocate(0) << 12);
        left_size = 4096 - size;
        current_ptr = result + size;
    }
    else {
        size = (size + 4095) & ~4095; // Align 4KB
        int size_pow = 0;
        while ((1 << (12 + size_pow)) < size) {
            size_pow++;
        }
        result = (char *)MOMORY_ALLOC_BASE + (buddy.Allocate(size_pow) << 12);
        current_ptr = nullptr;
        left_size = 0;
    }
    allocated_memory[allocated_memory_count++] = result;
    return result;
}

static inline bool sameChunk(char* ptr1, char* ptr2) {
    uint64_t a = uint64_t(ptr1) & ~(0x1000 - 1);
    uint64_t b = uint64_t(ptr2) & ~(0x1000 - 1);
    return a == b;
}

bool MemAlloc::free(char* ptr) {
    if (ptr == nullptr) {
        return false;
    }
    bool hasSameChunk = false;
    for (int i = 0; i < allocated_memory_count; i++) {
        if (allocated_memory[i] == ptr) {
            allocated_memory[i] = nullptr;
            allocated_memory_count--;
            if (i < allocated_memory_count) {
                allocated_memory[i] = allocated_memory[allocated_memory_count];
            }
        }
        else if (sameChunk(ptr, allocated_memory[i])) {
            hasSameChunk = true;
        }
    }
    if (!hasSameChunk) {
        if (sameChunk(ptr, current_ptr)) {
            current_ptr = nullptr;
        }
        buddy.Free((ptr - (char*)MOMORY_ALLOC_BASE) >> 12);
    }
    return true;
}

void MemAlloc::Init() {
    buddy.Init();
    allocated_memory_count = 0;
    allocated_memory = (char**)memset((char**)(MEM_ALLOC_PTR_BASE), 0, 0x1000);
    left_size = 0;
    current_ptr = 0;
}

MemAlloc allocator;

void* malloc(uint32_t size) {
    return allocator.malloc(size);
}
bool free(void* ptr) {
    return allocator.free((char*)ptr);
}
