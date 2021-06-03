.global switch_to
switch_to:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mrs x19, spsr_el1
    mrs x20, elr_el1
    mrs x21, sp_el0
    mov x9, sp
    stp x19, x20, [x0, 16 * 6]
    stp x21, x9, [x0, 16 * 7]

    ldp x21, x9, [x1, 16 * 7]
    ldp x19, x20, [x1, 16 * 6]
    msr spsr_el1, x19
    msr elr_el1, x20
    msr sp_el0, x21
    mov sp,  x9
    ldp fp, lr, [x1, 16 * 5]
    ldp x27, x28, [x1, 16 * 4]
    ldp x25, x26, [x1, 16 * 3]
    ldp x23, x24, [x1, 16 * 2]
    ldp x21, x22, [x1, 16 * 1]
    ldp x19, x20, [x1, 16 * 0]
    msr tpidr_el1, x1
    ret