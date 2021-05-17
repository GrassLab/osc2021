#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_
#include <stddef.h>

static inline void disable_interrupt() {
    asm("msr DAIFSet, 0xf");
}

static inline void enable_interrupt() {
    asm("msr DAIFClr, 0xf");
}

static inline size_t disable_irq_save() {
    size_t flags;
    asm(
        "mrs %0, DAIF\t\n"
        "msr DAIFSet, 0x2"
        :"=r"(flags)
    );

    return flags;
}

static inline void irq_restore(size_t flag) {
    asm(
        "msr DAIF, %0"
        ::"r"(flag)
    );
}

static inline void __wfi() {
    asm("wfi");
}

static inline void idle() {
    while(1) __wfi();
}

#endif