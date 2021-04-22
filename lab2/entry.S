
.globl el1_table_init
el1_table_init:
    adr    x0, el1_table
    msr    VBAR_EL1, x0
    ret

.macro entry label
  b \label
  .align  7
  .endm

.macro save_all
    sub sp, sp, 34 * 8
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
    mrs x22, SP_EL0
    mrs x23, elr_el1
    mrs x24, spsr_el1
    stp x30, x22, [sp, 16 * 15]
    stp x23, x24, [sp, 16 * 16]
.endm

.macro load_all
    ldp x23, x24, [sp, 16 * 16]
    ldp x30, x22, [sp, 16 * 15]
    msr spsr_el1, x24
    msr elr_el1,  x23
    msr SP_EL0,   x22
    ldp x28, x29, [sp ,16 * 14]
    ldp x26, x27, [sp ,16 * 13]
    ldp x24, x25, [sp ,16 * 12]
    ldp x22, x23, [sp ,16 * 11]
    ldp x20, x21, [sp ,16 * 10]
    ldp x18, x19, [sp ,16 * 9]
    ldp x16, x17, [sp ,16 * 8]
    ldp x14, x15, [sp ,16 * 7]
    ldp x12, x13, [sp ,16 * 6]
    ldp x10, x11, [sp ,16 * 5]
    ldp x8, x9, [sp ,16 * 4]
    ldp x6, x7, [sp ,16 * 3]
    ldp x4, x5, [sp ,16 * 2]
    ldp x2, x3, [sp ,16 * 1]
    ldp x0, x1, [sp ,16 * 0]
    add sp, sp, 34 * 8
.endm



.align 11
.global el1_table
el1_table:
// Exception from CurrentEL (SP_EL0)
    entry invalid_entry      // Synchronous
    entry invalid_entry      // IRQ
    entry invalid_entry      // FIQ
    entry invalid_entry      // SError
// Exception from CurrentEL (SP_ELx)
    entry el0_sync           // Synchronous
    entry irq_entry          // IRQ
    entry invalid_entry      // FIQ
    entry invalid_entry      // SError
// Exception from lower EL (AArch64)
    entry el0_sync           // Synchronous
    entry irq_entry          // IRQ
    entry invalid_entry      // FIQ
    entry invalid_entry      // SError
// Exception from lower EL (AArch32)
    entry invalid_entry      // Synchronous
    entry invalid_entry      // IRQ
    entry invalid_entry      // FIQ
    entry invalid_entry      // SError


.globl syscall_entry
syscall_entry:
    bl  syscall_handler
    str x0, [sp, #0]     // Now x0 is returned value of syscall_handler
    bl  chk_sched
    load_all
    eret
#define ESR_ELx_EC_SHIFT        26
#define ESR_ELx_EC_SVC64        0x15
el0_sync:
    save_all
    mov    x26, x0
    mov    x27, x1                 // set_trap_frame will polute x1 too
    mov    x0,  sp                 // borrow x0 as arg of set_trap_frame
    //bl imhere
    bl     set_trap_frame
    mov    x0,  x26                    // restore x0 as syscall arg
    mov    x1,  x27
    mrs    x25, esr_el1                // read the syndrome register
    lsr    x24, x25, #ESR_ELx_EC_SHIFT // exception class
    cmp    x24, #ESR_ELx_EC_SVC64      // SVC in 64-bit state
    b.eq   syscall_entry
    b invalid_handler

.globl irq_entry
irq_entry:
    save_all
    bl  irq_handler
    bl  chk_sched
    load_all
    eret

.globl invalid_entry
invalid_entry:
    b   invalid_handler

// [0]
.globl ret_from_fork
ret_from_fork:
    // bl  schedule_tail
    bl enable_irq
    cbz x19, ret_to_user
    mov x0, x20
    blr x19         //should never return
ret_to_user:
    load_all
    eret

//[0] https://github.com/s-matyukevich/raspberry-pi-os/blob/master/src/lesson04/src/entry.S#L146