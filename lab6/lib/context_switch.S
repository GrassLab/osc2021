#include <asm/constant.h>

.global switch_to
switch_to:
    mov x9, sp
    stp x19, x20, [x0, #0x0]
    stp x21, x22, [x0, #0x10]
    stp x23, x24, [x0, #0x20]
    stp x25, x26, [x0, #0x30]
    stp x27, x28, [x0, #0x40]
    stp x29, x9,  [x0, #0x50]
    str lr, [x0, #0x60]

    ldp x19, x20, [x1, #0x0]
    ldp x21, x22, [x1, #0x10]
    ldp x23, x24, [x1, #0x20]
    ldp x25, x26, [x1, #0x30]
    ldp x27, x28, [x1, #0x40]
    ldp x29, x9,  [x1, #0x50]
    ldr lr, [x1, #0x60]
    mov sp,  x9
    msr tpidr_el1, x1
    ret


.global restore_regs_eret
restore_regs_eret:
    ldp lr, x21, [sp, #0xf0]
    msr sp_el0, x21

    ldp x22, x23, [sp, #0x100]
    msr elr_el1, x22
	msr	spsr_el1, x23

    ldp x0, x1, [sp, #0]
    ldp x2, x3, [sp, #0x10]
    ldp x4, x5, [sp, #0x20]
    ldp x6, x7, [sp, #0x30]
    ldp x8, x9, [sp, #0x40]
    ldp x10, x11, [sp, #0x50]
    ldp x12, x13, [sp, #0x60]
    ldp x14, x15, [sp, #0x70]
    ldp x16, x17, [sp, #0x80]
    ldp x18, x19, [sp, #0x90]
    ldp x20, x21, [sp, #0xa0]
    ldp x22, x23, [sp, #0xb0]
    ldp x24, x25, [sp, #0xc0]
    ldp x26, x27, [sp, #0xd0]
    ldp x28, x29, [sp, #0xe0]

    add sp, sp, #PT_REGS_SIZE
    eret