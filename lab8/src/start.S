# include "start.h"

.section ".text.boot"

.global _start

_start:
    mrs    x1, mpidr_el1
    and    x1, x1, #3
    cbz    x1, 2f
1:
    wfe
    b      1b

2:
    ldr    x1, =0x80000
    mov    sp, x1
    
    // Disable MMU
    ldr    x1, =SCTLR_VALUE_MMU_DISABLED
    msr    sctlr_el1, x1
    // EL1 users aarch64
    ldr    x1, =HCR_EL2_VALUE
    msr    hcr_el2, x1

    // set EL2's SPSR
    ldr    x1, =SPSR_EL2_VALUE
    msr    spsr_el2, x1

    adr     x1, vector_table
    msr     vbar_el2, x1

    adr    x1, el1_start
    msr    elr_el2, x1

    eret

el1_start:
// set stack pointer
    ldr     x1, =0x60000
    mov     sp, x1
    
    // load exception_table to VBAR_EL1
    ldr     x1, =vector_table
    msr     vbar_el1, x1
    
    
    // setup TCR, MAIR, kernel page table
    ldr     x0, =TCR_CONFIG_DEFAULT
    msr     tcr_el1, x0
    ldr     x0, =MAIR_EL1_VALUE
    msr     mair_el1, x0
    
    ldr     x2, =kernel_page_setup
    ldr     x1, =KVA
    sub     x2, x2, x1
    blr     x2
    
    ldr     x0, =__kernel_page_start
    ldr     x1, =KVA
    sub     x0, x0, x1
    
    msr     ttbr0_el1, x0
    msr     ttbr1_el1, x0
    
    ldr     x1, =KVA
    add     sp, sp, x1
    
    // Enable MMU
    mrs     x2, sctlr_el1
    orr     x2, x2, 1
    msr     sctlr_el1, x2
    
    bl      clean_bss
    
    ldr     x2, =main
    blr     x2
    b       1b

clean_bss:
    ldr    x1, =__bss_start
    ldr    w2, =__bss_size
3:
    cbz    w2, 4f
    str    xzr, [x1], #8
    sub    w2, w2, #1
    cbnz   w2, 3b
4:    
    ret
