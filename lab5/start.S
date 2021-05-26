.section ".text.boot"

.global _start

_start:
    /* store dtb header */
    adr x5, DTB_HEADER
    str x0, [x5]

    /* let cores enter busy loop except core0 */
    mrs    x0, mpidr_el1
    and    x0, x0, 3
    cbz    x0, 3f
4:  wfe
    b      4b

3:
    bl set_exception_vector_table
    bl from_el2_to_el1
    // the next instruction runs in EL1

    /* set stack pointer */
    ldr x0, = _start
    mov sp, x0

    /* initialize bss to 0 */
    ldr x0, = __bss_start
    ldr w1, = __bss_size
    cbz w1, kernel_main
2:  str xzr, [x0], #8
    sub w1, w1, #1
    cbnz w1, 2b

1:
    bl kernel_main
    b 4b

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled, use SP_ELx
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1


.global DTB_HEADER

.data
    DTB_HEADER: .dword  0