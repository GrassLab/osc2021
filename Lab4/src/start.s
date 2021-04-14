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



.section ".text.boot"

.global _start

_start:
    mrs x1, MPIDR_EL1
    and x1, x1, #3
    cbz x1, 2f
    //if process id > 0 will loop
1:
    //boot
    wfe 
    b 1b

2:
	bl from_el2_to_el1
	ldr x0, =exception_table
	msr vbar_el1, x0 


    //let stack point  set to a proper address
    ldr x1, =_start
    mov sp, x1

    //clean bss

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    cbz x2, 4f
3:  str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, 3b
    //loop until bss clean
    
    //go to main function
4:  bl main
    b 1b

from_el2_to_el1:
	
	mov x0, (1 << 31)
	msr hcr_el2, x0
	mov x0, 0x3c5
	msr spsr_el2, lr 
	msr elr_el2, x30
	//執行 floating point 相關的指令時 就會進到 Synchronous Exception要清 CPACR_EL1.FPEN
	
	mov x0, #(3 << 20)
	msr cpacr_el1, x0
	eret


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
	b syn
	.align 7
	b irq
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


syn:
	save_all
	bl exception_handler
	load_all
	eret

irq:
	save_all
	bl interrupt_handler
	load_all
	eret

TODO:
	save_all
	bl error
	load_all
	eret