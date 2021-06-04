.section ".text.boot"



_start:
    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    bl  _from_el2_to_el1

    ldr x0, =_exception_vector_table
    msr VBAR_EL1, x0

    bl initMMU
    bl initMMULower

    // set stack before our code
    ldr     x1, =__stk_start
    mov     sp, x1



    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    b    3b


4:
    bl  main
    b   1b

_from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr

    mov     x0, #(3 << 20)
    msr     cpacr_el1, x0
    eret // return to EL1



