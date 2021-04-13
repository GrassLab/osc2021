#include "mm.h"
#include "exception.h"
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


.section ".text.boot"

.globl _start
_start:
    //ldr     x1, =0x100000
    //str     x0, [x1]            // store dt loading address to 0x100000
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF       // Check processor id
    cbz     x0, master          // Hang for all non-primary CPU
    b       proc_hang

proc_hang:
    wfe
    b       proc_hang

master:
    // Clear bss
    adr     x0, bss_begin
    adr     x1, bss_end
    sub     x1, x1, x0
    bl      memzero

    bl from_el2_to_el1  //jump to el1 exception level

	ldr x0, =exception_table
	msr VBAR_EL1, x0        //VBAR_EL1 stores the vector table.

    // Jump to C code
    mov     sp, #LOW_MEMORY
    bl      kernel_start



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
    eret // return to EL1

//exception table
.align 11       //align to 0x800
exception_table:
	b TODO
	.align 7    //0x080
	b TODO
	.align 7    //0x100
	b TODO
	.align 7    //0x180
	b TODO

	.align 7    //0x200
	b TODO
	.align 7    //0x280
	b TODO
	.align 7    //0x300
	b TODO
	.align 7    //0x380
	b TODO

	.align 7    //0x400 (here's for svm0 in requirement2)
	b TODO
	.align 7    //0x480 
	b TODO
	.align 7    //0x500
	b TODO
	.align 7    //0x580
	b TODO

	.align 7    //0x600
	b TODO
	.align 7    //0x680
	b TODO
	.align 7    //0x700
	b TODO
	.align 7    //0x780 
	b TODO

TODO:
	save_regs
	bl exception_handler
	load_regs
	eret