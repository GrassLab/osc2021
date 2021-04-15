.section ".text"
    .macro	kernel_entry
	sub	sp, sp, #256
	stp	x0, x1, [sp, #16 * 0]
	stp	x2, x3, [sp, #16 * 1]
	stp	x4, x5, [sp, #16 * 2]
	stp	x6, x7, [sp, #16 * 3]
	stp	x8, x9, [sp, #16 * 4]
	stp	x10, x11, [sp, #16 * 5]
	stp	x12, x13, [sp, #16 * 6]
	stp	x14, x15, [sp, #16 * 7]
	stp	x16, x17, [sp, #16 * 8]
	stp	x18, x19, [sp, #16 * 9]
	stp	x20, x21, [sp, #16 * 10]
	stp	x22, x23, [sp, #16 * 11]
	stp	x24, x25, [sp, #16 * 12]
	stp	x26, x27, [sp, #16 * 13]
	stp	x28, x29, [sp, #16 * 14]
	str	x30, [sp, #16 * 15] 
	.endm

    .macro	kernel_exit
	ldp	x0, x1, [sp, #16 * 0]
	ldp	x2, x3, [sp, #16 * 1]
	ldp	x4, x5, [sp, #16 * 2]
	ldp	x6, x7, [sp, #16 * 3]
	ldp	x8, x9, [sp, #16 * 4]
	ldp	x10, x11, [sp, #16 * 5]
	ldp	x12, x13, [sp, #16 * 6]
	ldp	x14, x15, [sp, #16 * 7]
	ldp	x16, x17, [sp, #16 * 8]
	ldp	x18, x19, [sp, #16 * 9]
	ldp	x20, x21, [sp, #16 * 10]
	ldp	x22, x23, [sp, #16 * 11]
	ldp	x24, x25, [sp, #16 * 12]
	ldp	x26, x27, [sp, #16 * 13]
	ldp	x28, x29, [sp, #16 * 14]
	ldr	x30, [sp, #16 * 15] 
	add	sp, sp, #256		
	eret
	.endm
    
    .macro	ventry	label
	.align	7
	b	   \label
	.endm

.align 11
.global exception_table
exception_table:
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

    ventry el1_exception
    ventry irq_handler
    ventry not_implemented
    ventry not_implemented

    ventry not_implemented
    ventry not_implemented
    ventry not_implemented
    ventry not_implemented

el1_exception:
    kernel_entry
    bl exception_handler
    kernel_exit

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable timer ENABLE, bit [0]
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1] // enable timer interrupt    
    ret


.global core_timer_handler
core_timer_handler:
    mrs x0, cntfrq_el0
	lsl x0, x0, 1
    msr cntp_tval_el0, x0
    ret

irq_handler:
	kernel_entry
	bl irq_router
	kernel_exit
 
//.globl enable_irq
//enable_irq:
//    msr   daifclr, #0xf
//    ret