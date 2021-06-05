#include "mmu.h"

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

    mov x0, 0 // PGD's page frame at 0x0
    mov x1, 0x1000 // PUD's page frame at 0x1000

    ldr x2, =BOOT_PGD_ATTR
    orr x2, x1, x2 // combine the physical address of next level page with attribute.
    str x2, [x0]

    ldr x2, =BOOT_PUD_ATTR
    mov x3, 0x00000000
    orr x3, x2, x3
    str x3, [x1] // 1st 1GB mapped by the 1st entry of PUD
    mov x3, 0x40000000
    orr x3, x2, x3
    str x3, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

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
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret

hang_loop:
    wfe
    b hang_loop
