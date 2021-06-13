.global from_el2_to_el1, from_el1_to_el0, core_timer_enable, asm_test, eret_initramfs

from_el2_to_el1:
    mov x0, 3 << 20     // Set 0b11 on FPEN bits of cpacr_el1 to disable trap for accessing SIMD registers
    msr cpacr_el1, x0

    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, x30
    bl core_timer_enable
    eret // return to EL1

from_el1_to_el0:
    //mov x0, 0x3c0 
    msr spsr_el1, x0
    mov x0, sp
    msr sp_el0, x0
    msr elr_el1, x30
    eret // return to EL0

eret_initramfs:
    msr elr_el1, x0
    msr sp_el0, x1
    mov x29, x2//fp
    mov x0, 0x340
    msr spsr_el1, x0
    mrs x3, sp_el0
    ldr x0, [x3, 0]
    ldr x1, [x3, 8]
    eret // return to EL0

core_timer_enable:
	mov x0, 1
	msr cntp_ctl_el0, x0
	mov x0, 0
	msr cntp_tval_el0, x0
	mov x0, 2
	ldr x1, =0x40000040
    str w0, [x1] // unmask timer interrupt
    ret

asm_test:
    mov x0, sp //set stack
    mov x1, 3
    bl print_regs
    eret // return to EL0

.global switch_to
switch_to:
    mov x1, x0
    mrs x0, tpidr_el1
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp x29, x30, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp x29, x30, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9
    msr tpidr_el1, x1
    ret

