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
	msr VBAR_EL1, x0 


    

    //clean bss

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    cbz x2, 4f
3:  str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, 3b
    //loop until bss clean
    
    //go to main function
4:  
	ldr x1, =__stack_top
    mov sp, x1
	bl main
    b 1b

from_el2_to_el1:
	
	mov x0, (1 << 31)
	msr hcr_el2, x0
	mov x0, 0x3c5
	msr spsr_el2, x0 
	msr elr_el2, x30
	//執行 floating point 相關的指令時 就會進到 Synchronous Exception要清 CPACR_EL1.FPEN
	
	mov x0, #(3 << 20)
	msr cpacr_el1, x0

	mov x0, 1
	msr cntp_ctl_el0, x0
	mov x0, 0
	msr cntp_tval_el0, x0
	mov x0, 2
	ldr x1, =0x40000040
	str w0, [x1]
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
	.align 7


syn:
	save_task
	bl exception_handler
	restore_task
	eret

irq:
	save_task
	bl interrupt_handler
	restore_task
	eret

TODO:
	save_task
	bl error
	restore_task
	eret