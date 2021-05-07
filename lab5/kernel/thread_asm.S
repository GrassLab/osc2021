.section ".text"

.global switch_to
.global get_tpidr_el1
.global kernel_thread_start
.global do_exit
.global fork_internal

from .req x0
to .req x1
to_tpidr .req x2
argc .req x3
argv .req x4

switch_to:
    cbz from, 1f
    stp x19, x20, [from, 16 * 0]
    stp x21, x22, [from, 16 * 1]
    stp x23, x24, [from, 16 * 2]
    stp x25, x26, [from, 16 * 3]
    stp x27, x28, [from, 16 * 4]
    stp fp, lr, [from, 16 * 5]
    mrs x9, sp_el0
    str x9, [from, 8 * 12]
    mrs x9, elr_el1
    str x9, [from, 8 * 13]
    mov x9, sp
    str x9, [from, 8 * 14]
1:
    ldp x19, x20, [to, 16 * 0]
    ldp x21, x22, [to, 16 * 1]
    ldp x23, x24, [to, 16 * 2]
    ldp x25, x26, [to, 16 * 3]
    ldp x27, x28, [to, 16 * 4]
    ldp fp, lr, [to, 16 * 5]
    ldr x9, [to, 8 * 12]
    msr sp_el0, x9
    ldr x9, [to, 8 * 13]
    msr elr_el1, x9
    ldr x9, [to, 8 * 14]
    mov sp, x9
    msr tpidr_el1, to_tpidr

    mov x0, x3
    mov x1, x4
    ret

get_tpidr_el1:
    mrs x0, tpidr_el1
    ret

// Do not touch x0, x1 argc and argv uses it as argument
kernel_thread_start:
    adr x2, do_exit
    mov lr, x2
    eret

do_exit:
    mov x8, #3
    svc #0

parent .req x0
child .req x1
parent_stack .req x2
child_stack .req x3
parent_kernel_stack .req x4
child_kernel_stack .req x5
fork_internal:
    
    stp fp, lr, [sp, #-64]!
    stp x0, x1, [sp, #16]
    stp x2, x3, [sp, #32]
    stp x4, x5, [sp, #48]

    ldr x0, [sp, #40]
    ldr x1, [sp, #32]
    mov x2, #4096
    bl memcpy
    ldr x0, [sp, #56]
    ldr x1, [sp, #48]
    mov x2, #4096
    bl memcpy

    ldp x0, x1, [sp, #16]
    ldp x2, x3, [sp, #32]
    ldp x4, x5, [sp, #48]
    ldp fp, lr, [sp], #64

    stp x19, x20, [parent, 16 * 0]
    stp x21, x22, [parent, 16 * 1]
    stp x23, x24, [parent, 16 * 2]
    stp x25, x26, [parent, 16 * 3]
    stp x27, x28, [parent, 16 * 4]
    stp fp, lr, [parent, 16 * 5]
    mrs x9, sp_el0
    str x9, [parent, 8 * 12]
    mrs x9, elr_el1
    str x9, [parent, 8 * 13]
    mov x9, sp
    str x9, [parent, 8 * 14]

    stp x19, x20, [child, 16 * 0]
    stp x21, x22, [child, 16 * 1]
    stp x23, x24, [child, 16 * 2]
    stp x25, x26, [child, 16 * 3]
    stp x27, x28, [child, 16 * 4]
    stp fp, lr, [child, 16 * 5]
    mrs x9, sp_el0
    sub x9, x9, x2
    add x9, x9, x3
    str x9, [child, 8 * 12]
    mrs x9, elr_el1
    str x9, [child, 8 * 13]
    mov x9, sp
    sub x9, x9, x4
    add x9, x9, x5
    str x9, [child, 8 * 14]

    ret

.global qemu_quit
qemu_quit:
    mov w0, #0x18
    mov x1, #0x26
    movk x1, #0x2, lsl #16
    str x1, [sp, #-16]!
    str xzr, [sp, #8]
    mov x1, sp
    hlt #0xf000
    b loop

.global get_timer
get_timer:
    mrs x0, cntpct_el0
    ret
