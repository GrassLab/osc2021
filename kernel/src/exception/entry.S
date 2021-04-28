#define CORE0_IRQ_SOURCE (0x40000060)

// x0 is thread addr, x1 is kernel sp addr
.macro get_current_kernel_sp
    mrs     x0, TPIDR_EL0
    ldr     x1, [x0]
.endm

// save general registers to stack
.macro save_reg
    // it's sp_el1
    sub sp, sp, 16 * 17
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    stp x30, fp, [sp, 16 * 15]
    str lr, [sp, 16 * 16]


    // update kernel sp
    get_current_kernel_sp
    sub     x1, x1, 16 * 17
    str     x1, [x0]
.endm

// load general registers from stack
.macro load_reg
    // update kernel sp
    get_current_kernel_sp
    add     x1, x1, 16 * 17
    str     x1, [x0]


    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldp x30, fp, [sp, 16 * 15]
    ldr lr, [sp, 16 * 16]
    add sp, sp, 16 * 17
.endm


.global exception_entry
exception_entry:
    // temporary save x0 x1 to set sp
    stp     x0, x1, [sp]
    mrs     x0, TPIDR_EL0
    mov     x1, sp

    // set sp to current thread's kernel sp
    ldr     x0, [x0]
    mov     sp, x0

    // restore x0 x1 from original sp
    ldp     x0, x1, [x1]

    save_reg


    // get the svc number, which is stored in lower 16 bit of ESR_ELx
    mrs     x0, ESR_EL1
    and     x0, x0, #0xFFFF
    bl      exception_handler

    /* restore registers from current thread
     * current thread might be different if schedule exception is taken
     */
    mrs     x0, TPIDR_EL0
    ldr     x1, [x0]
    mov     sp, x1

    // set user sp_el0
    ldr     x0, [x0, 8]
    msr     SP_EL0, x0

    load_reg

    msr     ELR_EL1, lr

    eret

.global irq_entry
irq_entry:
    save_reg

    ldr     x0, = CORE0_IRQ_SOURCE
    mrs     x1, cntpct_el0
    bl      irq_parser

    load_reg
    eret
