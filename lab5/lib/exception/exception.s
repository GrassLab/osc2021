.section ".data"

.section ".text"

.align 11 // aligned to 0x800 (2^11)
.global _exception_vector_table
_exception_vector_table:
    b _exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80 (2^7), .align will pad 0
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _irq_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7

    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7
    b _exception_handler
    .align 7


.macro _kernel_entry
sub sp, sp, #256 // size of all registers x0-x30 (31 * 16)
stp x0, x1, [sp, #16 * 0]
stp x2, x3, [sp, #16 * 1]
stp x4, x5, [sp, #16 * 2]
stp x6, x7, [sp, #16 * 3]
stp x8, x9, [sp, #16 * 4]
stp x10, x11, [sp, #16 * 5]
stp x12, x13, [sp, #16 * 6]
stp x14, x15, [sp, #16 * 7]
stp x16, x17, [sp, #16 * 8]
stp x18, x19, [sp, #16 * 9]
stp x20, x21, [sp, #16 * 10]
stp x22, x23, [sp, #16 * 11]
stp x24, x25, [sp, #16 * 12]
stp x26, x27, [sp, #16 * 13]
stp x28, x29, [sp, #16 * 14]
str x30, [sp, #16 * 15]
.endm

.macro _kernel_exit
ldp x0, x1, [sp, #16 * 0]
ldp x2, x3, [sp, #16 * 1]
ldp x4, x5, [sp, #16 * 2]
ldp x6, x7, [sp, #16 * 3]
ldp x8, x9, [sp, #16 * 4]
ldp x10, x11, [sp, #16 * 5]
ldp x12, x13, [sp, #16 * 6]
ldp x14, x15, [sp, #16 * 7]
ldp x16, x17, [sp, #16 * 8]
ldp x18, x19, [sp, #16 * 9]
ldp x20, x21, [sp, #16 * 10]
ldp x22, x23, [sp, #16 * 11]
ldp x24, x25, [sp, #16 * 12]
ldp x26, x27, [sp, #16 * 13]
ldp x28, x29, [sp, #16 * 14]
ldr x30, [sp, #16 * 15]
add sp, sp, #256
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

.global _exception_handler
_exception_handler:
    save_task

    bl _except_handler
    restore_task

    eret

.global _irq_handler
_irq_handler:
    save_task

    bl _timer_handler

    restore_task

    eret
