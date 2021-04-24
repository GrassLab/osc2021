.globl _load_user_program
_load_user_program:
    msr    elr_el1, x0
    msr    sp_el0, x1
    //mov    x0, 0x3c0
    mov    x0, 0
    msr    spsr_el1, x0
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
    save_all
    //mrs x0, spsr_el1
	//mrs x1, elr_el1
	//mrs x2, esr_el1
    //bl show_el1_status_info  
    bl sync_el0_handler
    load_all
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

