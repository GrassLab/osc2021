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
    
    //bl     core_timer_init

    eret

el1_start:
// set stack pointer
    ldr     x1, =0x60000
    mov     sp, x1
    
    // load exception_table to VBAR_EL1
    adr     x1, vector_table
    msr     vbar_el1, x1
    
    // setup TCR, MAIR, mmu
    ldr     x0, =TCR_CONFIG_DEFAULT
    msr     tcr_el1, x0
    ldr     x0, =MAIR_EL1_VALUE
    msr     mair_el1, x0
    bl      mmu_setup
    
    bl      clean_bss
    
    ldr     x1, =KVA
    add     sp, sp, x1
    
    ldr     x2, =main
    br      x2
    b       1b

mmu_setup:
    mov     x0, 0x0 // PGD's page frame at 0x0
    mov     x1, 0x1000 // PUD's page frame at 0x1000

    ldr     x2, = BOOT_PGD_ATTR
    orr     x2, x1, x2 // combine the physical address of next level page with attribute.
    str     x2, [x0]

    ldr     x2, = BOOT_PUD_DEV_ATTR
    mov     x3, 0x00000000
    orr     x3, x2, x3
    str     x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    
    ldr     x2, = BOOT_PUD_DEV_ATTR
    mov     x3, 0x40000000
    orr     x3, x2, x3
    str     x3, [x1, #8] // 2nd 1GB mapped by the 2nd entry of PUD
    
    msr     ttbr0_el1, x0 // load PGD to the bottom translation based register.
    msr     ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs     x2, sctlr_el1
    orr     x2 , x2, 1
    msr     sctlr_el1, x2 // enable MMU, cache remains disabled
    
    ret
    
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
