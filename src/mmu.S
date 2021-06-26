#include "mmu.h"

.global set_mmu
set_mmu:
    /* Set TC Register */
    ldr   x0, =TCR_EL1_VALUE
    msr   tcr_el1, x0

    /* Set MAI Register */
    ldr   x0, =MAIR_EL1_VALUE
    msr   mair_el1, x0

    /* Set page table and map kernel */
    ldr   x0, =PGD_ADDR
    ldr   x1, =PUD_ADDR
    ldr   x2, =PMD_ADDR

    /* PGD */
    ldr   x3, =PGD_ATTR
    orr   x3, x1, x3
    str   x3, [x0]

    /* PUD */
    ldr   x3, =PUD_ATTR
    orr   x3, x2, x3
    str   x3, [x1]
    ldr   x3, =PUD_DEVICE_ATTR
    mov   x4, 0x40000000
    orr   x3, x4, x3
    str   x3, [x1, 8]

    /* PMD */
    mov   x1, #0
1:  ldr   x3, =PMD_NORMAL_ATTR
    orr   x3, x1, x3
    str   x3, [x2], #8
    add   x1, x1, 0x200000
    ldr   x4, =0x3f000000
    sub   x4, x4, x1
    cbnz  x4, 1b
2:  ldr   x3, =PMD_DEVICE_ATTR
    orr   x3, x1, x3
    str   x3, [x2], #8
    add   x1, x1, 0x200000
    ldr   x4, =0x40000000
    sub   x4, x4, x1
    cbnz  x4, 2b

    msr   ttbr0_el1, x0
    msr   ttbr1_el1, x0

    mrs   x2, sctlr_el1
    orr   x2, x2, 1
    msr   sctlr_el1, x2
    ret
