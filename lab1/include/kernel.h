#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

class Kernel {
public:
    static void Reset(uint32_t tick);
    inline static void Delay(uint64_t cycle) {
        asm volatile("__delay_%=: subs %[cycle], %[cycle], #1\n bne __delay_%=\n"
            : "=r"(cycle): [cycle]"0"(cycle) : "cc");
    }
};
#endif
