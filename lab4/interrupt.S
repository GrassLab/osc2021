// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
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
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
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
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

invalid_handler:
    bl invalid_entry
    eret

el0_sync_handler:
    bl disable_irq
    save_all
    bl el0_sync_entry
    load_all
    bl enable_irq
    eret

irq_handler:
    bl disable_irq
    save_all
    bl irq_entry
    load_all
    bl enable_irq
    eret

.align 11 // vector table should be aligned to 0x800 (11bits)
.global exception_vector_table
exception_vector_table:
    /* Exception from CurrentEL (SP_EL0) (EL1 -> EL1) */
    b invalid_handler   // Synchronous
    .align 7
    b invalid_handler   // IRQ
    .align 7
    b invalid_handler   // FIQ
    .align 7
    b invalid_handler   // SError
    .align 7

    /* Exception from CurrentEL (SP_ELx) (EL1 -> EL1) */
    b invalid_handler   // Synchronous
    .align 7
    b invalid_handler   // IRQ
    .align 7
    b invalid_handler   // FIQ
    .align 7
    b invalid_handler   // SError
    .align 7

    /* Exception from lower EL (AArch64) (ELx -> ELy, x < y) */
    b el0_sync_handler   // Synchronous
    .align 7
    b irq_handler   // IRQ
    .align 7
    b invalid_handler   // FIQ
    .align 7
    b invalid_handler   // SError
    .align 7

    /* Exception from lower EL (AArch32) (ELx -> ELy, x < y) */
    b invalid_handler   // Synchronous
    .align 7
    b invalid_handler   // IRQ
    .align 7
    b invalid_handler   // FIQ
    .align 7
    b invalid_handler   // SError
    .align 7

.global set_exception_vector_table
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    ret

.globl enable_irq
enable_irq:
    msr    daifclr, #2
    ret

.globl disable_irq
disable_irq:
    msr    daifset, #2
    ret