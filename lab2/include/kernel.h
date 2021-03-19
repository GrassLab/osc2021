#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

class Kernel {
public:
    static void Reset(uint32_t tick);
    inline static void Delay(uint64_t cycle) {
        asm volatile("subs %x[cycle], %x[cycle], #1\n"
                     "bne -4"
                     : "=r"(cycle)
                     : [cycle] "0"(cycle >> 1)
                     : "cc");
    }
};
#endif
