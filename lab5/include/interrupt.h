#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

static inline void disable_interrupt() {
    asm("msr DAIFSet, 0xf");
}

static inline void enable_interrupt() {
    asm("msr DAIFClr, 0xf");
}

#endif