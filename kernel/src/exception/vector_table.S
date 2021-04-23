.global exec_in_el0
.global el1_exception_vector_table
.global set_el1_exception_vector_table

.align 11 // vector table should be aligned to 0x800
el1_exception_vector_table:
    // Current EL with SP0
    b noop /* Synchronous */ 
    .align 7 /* entry size is 0x80, .align will pad 0 */
    b noop /* IRQ/vIRQ */
    .align 7
    b noop /* FIQ/vFIQ */
    .align 7
    b noop /* SError/vSError */
    .align 7

    // Current EL with SPx
    b noop
    .align 7
    b noop
    .align 7
    b noop
    .align 7
    b noop
    .align 7

    // Lower EL using AArch64
    b exception_entry
    .align 7
    b irq_entry
    .align 7
    b noop
    .align 7
    b noop
    .align 7

    // Lower EL using AArch32 
    b noop
    .align 7
    b noop
    .align 7
    b noop
    .align 7
    b noop
    .align 7


set_el1_exception_vector_table:
    adr   x0, el1_exception_vector_table
    msr   vbar_el1, x0
    ret   lr

exec_in_el0:
    msr   elr_el1, x0
    mov   x1, 0
    msr   spsr_el1, x1 

    // set sp of el0
    add   x2, x0, 0x10000
    msr   sp_el0, x2 
    
    bl    core_timer_enable
    eret
    
noop: 
    eret