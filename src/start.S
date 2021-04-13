#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3              // get CPU ID and mask reserved bits
    cbz     x1, clear_bss
    // cpu id > 0, stop
proc_hang:  
    wfe                             // hang for all noo-primary CPU and enter low power mode
    b       proc_hang    

clear_bss:
    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
clear_loop:  
    cbz     w2, master
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, clear_loop          // Whether bss section is not all set to zero

master:  
    // cpu id == 0, is primary CPU 
    ldr     x0, =SCTLR_VALUE_MMU_DISABLED
    msr     sctlr_el1, x0

    ldr     x0, =HCR_EL2_VALUE       // set EL1 aarch as aarch 64
    msr     hcr_el2, x0

    ldr	    x0, =SPSR_EL2_VALUE      // set processor restore state after calling eret
    msr	    spsr_el2, x0

    adr	    x0, el1_entry		     // eret return address
    msr	    elr_el2, x0

    eret		                     // from el2 to el1

el1_entry:
    ldr     x1, =CPACR_EL1_VALUE     // Make el0, el1 can use Floating point and Advanced SIMD
    msr     CPACR_EL1, x1

    ldr     x1, =_start               // set EL1 stack before our code
    mov     sp, x1

    ldr     x0, =exception_vector_table      // set up el1 exception table
    msr     vbar_el1, x0

    //ldr     x0, =SPSR_EL1_VALUE
    //msr     spsr_el1, x0

    ldr	    x0, =0x3c5                // set processor restore state after calling eret
    msr	    spsr_el1, x0              // kernel should be running in EL1

    ldr     x1, =0x40000              // set EL0 stack pointer
    msr     sp_el0, x1

    adr     x0, pseudo_main
    msr     elr_el1, x0

    eret                              

/* Now kernel should be running in EL1 and jump to main function*/
pseudo_main:  
    bl      main                      // jump to main functoin in C code, should not return
    b       proc_hang                 // for failsafe, halt this core too (Should never do this instruction)