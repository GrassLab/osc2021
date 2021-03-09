#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

class Kernel {
public:
    static void Reset(uint32_t tick);
    inline static void Delay(uint64_t cycle) {
        while (cycle--) asm volatile("");
    }
};
#endif
