# include "start.h"
# include "mem_addr.h"

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
    ldr    x1, =__stack_top
    mov    sp, x1
    ldr    x1, =__bss_start
    ldr    w2, =__bss_size
3:
    cbz    w2, 4f
    str    xzr, [x1], #8
    sub    w2, w2, #1
    cbnz   w2, 3b

4:
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

    // mask all interrupt, and set exception level to el0
    ldr     x1, =SPSR_EL1_VALUE
    msr     spsr_el1, x1

    adr     x1, el0_start
    msr     elr_el1, x1

    ldr     x1, =0x40000
    msr     sp_el0, x1

    bl     main
    b      1b

    eret

el0_start:
    ldr     x1, =0x40000
    mov     sp, x1

    bl      main
    b       1b

