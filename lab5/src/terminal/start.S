.section ".text.boot"
_start:
    bl from_el2_to_el1
    adr x0, _start
    sub sp, x0, 0x10000
    adr x0, exception_vector_table
    msr vbar_el1, x0
    bl from_el1_to_el0
    adr x0, _start
    adr x1, __bss_start__
    adr x2, __bss_end__
    mov sp, x0
set_bss_zero_loop:
    cmp x2, x1
    bls entry
    ldr xzr, [x1], #8
    b set_bss_zero_loop
entry:
    bl main
loop:
    wfe
    b loop
from_el2_to_el1:
    mov x0, (1 << 31)
    msr hcr_el2, x0
    mov x0, 0x3c5
    msr spsr_el2, x0
    msr elr_el2, lr
    eret
from_el1_to_el0:
    mov x0, 0x200000
    msr sp_el0, x0
    mov x0, 0x3c0
    msr spsr_el1, x0
    msr elr_el1, lr
    eret
