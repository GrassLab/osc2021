.section ".text.start"

_start: 
    ldr     x0, = bss_begin
    ldr     x1, = bss_end
    sub     x1, x1, x0
    bl      clear
    
    bl      set_exception_vector_table
    bl      from_el2_to_el1
    // EL1 start here

    mrs     x1, cpacr_el1
    orr     x1, x1, (3 << 20) // # #0b1100110000000000000000
    msr     cpacr_el1, x1

    ldr     x0, = _stack_top
    mov     sp, x0
    b       kernel

clear:
    // clear bss memory
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear
    ret     lr
    

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1
