#include "sys_regs.h"

.section ".text.boot"

.global _start
_start:
    // read cpu id, stop slave cores
    // read the content of mpidr_el1, indicating the core number, into x1
    mrs     x1, mpidr_el1
    // keep only the first and the second bit
    and     x1, x1, #3
    // jump to el2_entry if x0 is zero
    cbnz    x1, hang_on
    
    ldr     x0, =_start
    mov     sp, x0

el2_entry:
    ldr     x1, =HCR_EL2_VALUE    // set EL1 aarch
    msr     hcr_el2, x1

    ldr     x1, =SPSR_EL2_VALUE   // set processor restore state after calling eret  
    msr     spsr_el2, x1

    # init sp for EL1
    mov     x0, #0x70000
    msr     sp_el1, x0

    adr     x1, el1_entry         // eret return address
    msr     elr_el2, x1

    eret

el1_entry:
    ldr     x0, =exception_vector_table  // set up el1 exception table
    msr     vbar_el1, x0

    // clear bss
    ldr     x1, =_bss_start
    ldr     w2, =_bss_size

clear_loop:
    cbz     w2, pseudo_main
    # set the content x1 points to as zero
    # and x1 = x1 + 8
    str     xzr, [x1], #8
    # w2 = w2 - 1
    sub     w2, w2, #1
    cbnz    w2, clear_loop

pseudo_main:
    // jump to C code, should not return
    // bl is to branch with the address of the next instruction saved, 
    // whereas b is to simply branch and will not return
    bl      main
    // for failsafe, halt this core too
    b       hang_on

hang_on:
    // cpu id > 0, stop
    // wfe puts the core into a low-power standby mode
    wfe
    // an infinity loop forms here
    b       hang_on