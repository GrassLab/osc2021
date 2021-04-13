#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    //ldr     x1, =0x100000
    //str     x0, [x1]          // store dt loading address to 0x100000
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF       // Check processor id
    cbz     x0, master          // Hang for all non-primary CPU
    b       proc_hang

proc_hang:
    wfe
    b       proc_hang

master:
    // init exception level
    bl      from_el2_to_el1

    // init exception table
    ldr     x0, =exception_vector_table
    msr     vbar_el1, x0

    // Clear bss
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero

    // Jump to C code
    mov     sp, #LOW_MEMORY
    bl      kernel_start
    b       proc_hang

.macro save_regs
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_regs
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

from_el2_to_el1:
    mov     x0, (1 << 31)   // EL1 uses aarch64
    msr     hcr_el2, x0
    mov     x0, 0x3c5       // EL1h (SPSel = 1) with interrupt disabled
    msr     spsr_el2, x0
    msr     elr_el2, lr
    eret                    // return to EL1


.align 11                   // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
    b       TODO            // branch to a handler function.
    .align 7                // entry size is 0x80, .align will pad 0
    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7

    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7

    b       lowerSync64
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7

    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7
    b       TODO
    .align 7


lowerSync64:
    save_regs
    bl      exception_handler
    load_regs
    eret

TODO:
    save_regs
    bl      non_implement_handler
    load_regs
    eret