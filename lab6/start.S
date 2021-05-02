.macro save_regs
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

.macro load_regs
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

.section ".text.boot"

_start:
//cpu init
	mrs x1, mpidr_el1//read cpu id
	and x1, x1, #3
	cbz x1, 2f
1://id>0, stop
	wfe
	b 1b
2://id=0, exec

//exception level init
	bl from_el2_to_el1

//exception table init
	ldr x0, =exception_table
	msr VBAR_EL1, x0

//bss init
	ldr x1, =__bss_start//64-bit load
	ldr w2, =__bss_size//32-bit load
3:
	cbz w2, 4f
	str xzr, [x1], #8//64-bit store
	sub w2, w2, #1
	b 3b
4:

//stack init
	ldr x1, =__stk_start
	mov sp, x1

//jump to main
	bl main//function call

//for failsafe
	b 1b

//el2->el1
from_el2_to_el1:
	mov x0, (1 << 31) // EL1 uses aarch64
	msr hcr_el2, x0
	mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
	msr spsr_el2, x0
	msr elr_el2, x30//x30 = link register

	// disable SIMD traps: built-ins of uart_printf will use SIMD
	// https://github.com/bztsrc/raspi3-tutorial/tree/master/12_printf
	mov x0, #(3 << 20)
	msr cpacr_el1, x0

	//enable the core timer’s interrupt
	mov x0, 1
	msr cntp_ctl_el0, x0
	mov x0, 0
	msr cntp_tval_el0, x0
	mov x0, 2
	ldr x1, =0x40000040
	str w0, [x1]

	eret // return to EL1

//exception table
.align 11
exception_table:
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO

	.align 7
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO

	.align 7
	b lowerSynchronous
	.align 7
	b lowerIRQ
	.align 7
	b TODO
	.align 7
	b TODO

	.align 7
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO
	.align 7
	b TODO

//Synchronous from lower level
lowerSynchronous:
	save_task
	bl exception_handler
	restore_task
	eret

//IRQ from lower level
lowerIRQ:
	save_task
	bl interrupt_handler
	restore_task
	eret

TODO:
	save_task
	bl error_handler
	restore_task
	eret