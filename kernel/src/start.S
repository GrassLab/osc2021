#include "mmu_values.h"

.section ".text.boot"

.global _start

_start:
    // get cpu id
    mrs x1, mpidr_el1
    and x1, x1, #0xff
    // only the primary cpu (id = 0) goes to master function
    cbz x1, master
    b hang_loop

master:
    // save dtb loading address
    //ldr x1, =0x9000000
    //str x0, [x1]

    bl from_el2_to_el1
    bl set_exception_vector_table

set_virtual_memory:
    ldr x0, =TCR_CONFIG_DEFAULT
    msr tcr_el1, x0

    ldr x0, =( \
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | \
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)) \
    )
    msr mair_el1, x0

    ldr x0, =PGD_BASE // PGD's page frame at 0x0
    ldr x1, =PUD_BASE // PGD's page frame at 0x0
    ldr x2, =PMD_BASE // PGD's page frame at 0x0
    ldr x3, =PTE_BASE // PGD's page frame at 0x0

    ldr x4, =BOOT_PGD_ATTR
    mov x5, x1
    orr x6, x5, x4 // combine the physical address of next level page with attribute.
    str x6, [x0]

    ldr x4, =BOOT_PUD_ATTR
    mov x5, x2
    orr x6, x5, x4
    str x6, [x1] // 1st 1GB mapped by the 1st entry of PUD
    add x5, x5, #0x1000
    orr x6, x5, x4
    str x6, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

    ldr x4, =BOOT_PMD_ATTR
    mov x5, x3
    mov x7, x2
    mov x9, #(512 * 2)
set_PMD:
    orr x6, x5, x4
    str x6, [x7] // 2MB block
    sub x9, x9, #1
    add x7, x7, #8
    add x5, x5, #0x1000
    cbnz x9, set_PMD

    ldr x4, =BOOT_PTE_NORMAL_NOCACHE_ATTR
    mov x5, #0x00000000
    mov x7, x3
    mov x9, #(512 * 512 * 2)
    ldr x10, =PERIPHERAL_BASE
set_PTE:
    cmp x5, x10
    blt normal_mem
    ldr x4, =BOOT_PTE_DEVICE_nGnRnE_ATTR
normal_mem:
    orr x6, x5, x4
    str x6, [x7] // 4KB page
    sub x9, x9, #1
    add x7, x7, #8
    add x5, x5, #(1 << 12)
    cbnz x9, set_PTE

    msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
    msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled

    ldr x2, =boot_rest // indirect branch to the virtual address
    br x2

boot_rest:
    // clear bss segment
    ldr x0, =__bss_start
    ldr x1, =__bss_size

clear_bss_start:
    cbz x1, clear_bss_done  // if remaining bss size == 0
    str xzr, [x0], #8       // *x0 = 0 (xzr register stores 0), x0 += 8
    sub x1, x1, #1
    cbnz x1, clear_bss_start

clear_bss_done:
    // set stack pointer
    ldr x1, =_start
    mov sp, x1

    bl main                 // jump to kernel main (c code)
    b hang_loop             // jump to endless loop if main returns (for fail-safe)

from_el2_to_el1:
    mov x0, (1 << 31)       // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5           // EL1h with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr

    // IMPORTANT: disable exceptions of accessing the SIMD and floating-point registers
    mov x0, #(3 << 20)
	msr cpacr_el1, x0

    eret                    // return to EL1

set_exception_vector_table:
    // adr x0, exception_vector_table
    ldr x0, =exception_vector_table
    msr vbar_el1, x0
    ret

hang_loop:
    wfe
    b hang_loop
