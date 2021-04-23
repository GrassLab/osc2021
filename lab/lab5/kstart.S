.section ".text"
.global _start

_start:
	/* read cpu id, stop slave cores */
	mrs    x1, mpidr_el1 // mpidr_ell: cpu core/device information
	and    x1, x1, #0xff // mask information to get cpu id

	/* jump to set up stack pointer */
	cbz    x1, _setup_stack // if cpu id == 0 --> go to set up stack pointer

_error:  
	wfe
	b	_error

_setup_stack:
	bl _from_el2_to_el1
	bl _set_exception_vector_table
	ldr 	x1, =_stack_top
	mov 	sp, x1
	// mov sp, #0x80000

_setup_bss:
	ldr     x1, =_bss_top
	ldr     w2, =bss_size

_init_bss:
	cbz     w2, _main		// if there is no bss --> go to run main
	str     xzr, [x1], #8 // zxr is a register always contain 0
	sub     w2, w2, #1
	cbnz    w2, _init_bss

_main:
	bl	main // jump to C code, should not return
	b		_error // for failsafe, halt this core too

_from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    mov x0, #(3 << 20)
	  msr cpacr_el1, x0
    eret // return to EL1


.align 11 // vector table should be aligned to 0x800
.global _exception_vector_table 
_exception_vector_table:
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0

	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _el1_irq_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0

	b _el0_sync_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _el0_irq_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0

	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
	b _default_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0

  

_set_exception_vector_table:
	adr x0, _exception_vector_table
	msr vbar_el1, x0
	ret


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

.extern print_h
.macro print_reg register
	save_all
	mov x0, \register
	bl print_h
	load_all
.endm

/* handler block */
_default_handler:
    eret

_el0_sync_handler:
    save_all
    bl _el0_sync_entry
    load_all
    eret

_el0_sync_entry:
    mrs x0, elr_el1
    print_reg x0
    ret
    mrs x0, spsr_el1
    print_reg x0
    mrs x0, esr_el1
    print_reg x0
    ret

.extern _el0_irq_entry
_el0_irq_handler:
    save_all
    bl _el0_irq_entry
    // bl _core_timer_handler
    load_all
    eret


#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global _timer_enable
_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    mov x0, 2
    str w0, [x1] // unmask timer interrupt
    ret

.globl _timer_disable
_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0
    mov x0, 0
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1]          // disable timer interrupt
    ret

.globl _timer_add
_timer_add:
	// mrs x1, cntfrq_el0
	// mul x0, x0, x1
    msr cntp_tval_el0, x0
    ret

.extern _timer_handler
_core_timer_handler:
    mrs x0, cntpct_el0
    print_reg x0
    mrs x0, cntfrq_el0
    print_reg x0

    save_all
    // bl _timer_disable
    bl _timer_handler
    load_all
    ret

// .extern _irq_entry
_el1_irq_handler:
	save_all
	// bl _el1_irq_entry
	load_all
	eret


/* irq enable / disable */
.globl _irq_enable // enable interrupt
_irq_enable:
	msr DAIFClr, 0xf
	ret

.globl _irq_disable // disable interrupt
_irq_disable:
	msr DAIFSet, 0xf
	ret


/* thread and process */
.global switch_to
switch_to:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp fp, lr, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9
    msr tpidr_el1, x1
    ret

.global get_current
get_current:
    mrs x0, tpidr_el1
    ret