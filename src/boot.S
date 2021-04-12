#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    //ldr     x1, =0x100000
    //str     x0, [x1]            // store dt loading address to 0x100000
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF       // Check processor id
    cbz     x0, master          // Hang for all non-primary CPU
    b       proc_hang

proc_hang:
    wfe
    b       proc_hang

master:
    bl      from_el2_to_el1

    // Clear bss
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero

    // Jump to C code
    mov     sp, #LOW_MEMORY
    bl      kernel_start

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1