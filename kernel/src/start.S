#include "kernel.h"
.section ".text.start"

_start: 
    // set vector table and switch to el1
    bl      set_el1_exception_vector_table
    bl      from_el2_to_el1

    /* !!! EL1 start here !!! */

    /* 
     * CPACR_EL1.FPEN = 11
     * Does not cause any instruction to be trapped(to EL1) when
     * EL0 and EL1 accesses to the SIMD and floating-point registers
     * since printf use va_arg() macro which has to access SIMD and FP regesters
     */ 
    mrs     x1, CPACR_EL1
    orr     x1, x1, (3 << 20)
    msr     CPACR_EL1, x1

    // clear bss and set stack top before c function call
    ldr     x0, = bss_begin
    ldr     x1, = bss_end
    sub     x1, x1, x0
    bl      clear_bss

    ldr     x0, = KERNEL_STACK_TOP
    mov     sp, x0
    
    #bl      idle
    bl      kernel_main

from_el2_to_el1:
    /*
     * HCR_EL2.RW control the execution state for lower Exception levels
     * Exceution state of EL1 is aarch64
     */
    mov     x0, (1 << 31) 
    msr     HCR_EL2, x0

    /*
     * SPSR_EL2 holds the saved process state when an exception is taken to EL2.
     * Since we are already in EL2 by default, before returning to EL1, we have to 
     * manually set the PSTATE that EL1 can restore
     */
    mov     x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr     SPSR_EL2, x0
    msr     ELR_EL2, lr
    
    eret