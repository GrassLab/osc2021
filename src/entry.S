.globl _load_user_program
_load_user_program:
    msr    elr_el1, x0
    msr    sp_el0, x1
    mov    x2, 0x340
    msr    spsr_el1, x2
    mrs    x3, sp_el0 
    ldr x0, [x3, 0]    // main argc
    ldr x1, [x3, 8]    // main argv
    eret

.macro print_el registers
	sub sp, sp, 32
	str lr, [sp]
	mov x0, \registers
    bl uart_printhex
	ldr lr, [sp]
	add sp, sp, 32
.endm

//.macro 
show_exception_status_info:
    sub sp, sp, 32
	str lr, [sp]
    
    mrs x0, spsr_el1
	print_el x0
	mrs x0, elr_el1
	print_el x0
	mrs x0, esr_el1
	print_el x0

    ldr lr, [sp]
	add sp, sp, 32
	ret
//.endm

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

.macro save_task
	str x0, [sp, -8]
	mrs x0, tpidr_el1
	//store x0&x1 later
	stp x2, x3, [x0, 8 * 18]
	stp x4, x5, [x0, 8 * 20]
	stp x6, x7, [x0, 8 * 22]
	stp x8, x9, [x0, 8 * 24]
	stp x10, x11, [x0, 8 * 26]
	stp x12, x13, [x0, 8 * 28]
	stp x14, x15, [x0, 8 * 30]
	stp x16, x17, [x0, 8 * 32]
	stp x18, x19, [x0, 8 * 34]
	stp x20, x21, [x0, 8 * 36]
	stp x22, x23, [x0, 8 * 38]
	stp x24, x25, [x0, 8 * 40]
	stp x26, x27, [x0, 8 * 42]
	stp x28, x29, [x0, 8 * 44]
	str x30, [x0, 8 * 46]
	mov x9, x0
	ldr x0, [sp, -8]
	stp x0, x1, [x9 ,8 * 16]
	mrs x10, spsr_el1
	mrs x11, elr_el1
	mrs x12, sp_el0
	str x10, [x9, 8 * 13]
	stp x11, x12, [x9, 8 * 14]
.endm

.macro restore_task
	mrs x9, tpidr_el1
	ldr x10, [x9, 8 * 13]
	ldp x11, x12, [x9, 8 * 14]
	msr spsr_el1, x10
	msr elr_el1, x11
	msr sp_el0, x12
	mov x0, x9
	//restore x0&x1 later
	ldp x2, x3, [x0, 8 * 18]
	ldp x4, x5, [x0, 8 * 20]
	ldp x6, x7, [x0, 8 * 22]
	ldp x8, x9, [x0, 8 * 24]
	ldp x10, x11, [x0, 8 * 26]
	ldp x12, x13, [x0, 8 * 28]
	ldp x14, x15, [x0, 8 * 30]
	ldp x16, x17, [x0, 8 * 32]
	ldp x18, x19, [x0, 8 * 34]
	ldp x20, x21, [x0, 8 * 36]
	ldp x22, x23, [x0, 8 * 38]
	ldp x24, x25, [x0, 8 * 40]
	ldp x26, x27, [x0, 8 * 42]
	ldp x28, x29, [x0, 8 * 44]
	ldr x30, [x0, 8 * 46]
	ldp x0, x1, [x0, 8 * 16]
.endm

.macro exception_entry label
    .align 7
    b     \label                    // branch to a handler function.
    
.endm

.align 11                           // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
    exception_entry default_exception_handler
    exception_entry default_exception_handler
    exception_entry default_exception_handler
    exception_entry default_exception_handler   

    exception_entry sync_el1_handler
    exception_entry irq_el1_handler
    exception_entry default_exception_handler
    exception_entry default_exception_handler 

    exception_entry sync_el0_aarch64
    exception_entry irq_el0_aarch64
    exception_entry default_exception_handler
    exception_entry default_exception_handler 

    exception_entry default_exception_handler
    exception_entry default_exception_handler
    exception_entry default_exception_handler
    exception_entry default_exception_handler    

sync_el1_handler:
    save_all
    mrs x0, esr_el1
    mrs x1, elr_el1
    bl handle_sync_el1
    load_all
    eret

irq_el1_handler:
    save_all
    bl handle_el1_irq
    load_all
    eret

irq_el0_aarch64:
    save_all
    bl handle_el0_irq
    load_all
    eret

sync_el0_aarch64:
    //save_all 
    save_task
    bl sync_el0_handler
    //load_all
    restore_task
    eret

default_exception_handler:
    //save_all
    mrs x0, spsr_el1
	mrs x1, elr_el1
	mrs x2, esr_el1
    bl show_el1_status_info  
    //b err_hang
    //load_all
    eret

err_hang:
    b err_hang

.global set_exception_vector_table
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret

.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

