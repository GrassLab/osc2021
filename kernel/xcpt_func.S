.global eret_user

eret_user:
    mov x0, 0x3c0
    msr spsr_el1, x0
    mov x0, 0x100000
    msr elr_el1, x0
    mov x0, 0x110000
    msr sp_el0, x0
    eret