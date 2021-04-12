.global exec_in_el0

exec_in_el0:
    msr     elr_el1, x0
    mov     x1, 0x3c0
    msr     spsr_el1, x1
    add     x2, x0, 0x10000
    msr     sp_el0, x2
    eret
