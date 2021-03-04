#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs     x0, mpidr_el1       //load syntax register into general purpose register
    and     x0, x0, #0xFF       // Check processor id
    cbz     x0, master          // Hang for all non-primary CPU (cbz: compare and branch on zero)
    b       proc_hang

proc_hang:
    wfe                         //put the processor into low-power mode, but not effecting the clock of processor (something like idle)
    b       proc_hang

master:
    // Clear bss
    adr     x0, bss_begin       //defined in linker script
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero             //memzero takes two parameters as input, that is the begining address and the size you want to clear.

    // Jump to C code
    mov     sp, #LOW_MEMORY     //initialing stack pointer
    bl      kernel_start