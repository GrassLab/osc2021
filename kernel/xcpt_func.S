#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global from_el2_to_el1
from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, x30
    mov x0, #(3 << 20)
	msr cpacr_el1, x0
    eret // return to EL1

.global from_el1_to_el0
from_el1_to_el0:
    mov x0, 0
    msr spsr_el1, x0
    mov x0, sp
    msr sp_el0, x0
    msr elr_el1, x30
    eret // return to EL0

.global run_user_program
run_user_program:
    mov x2, x0
    msr elr_el1, x2 // eret will goto elr_el1
    mov x2, x1
    msr sp_el0, x2 // point to stack
    mov x2, 0x340  // disable interrupt 0x3c0
    msr spsr_el1, x2
    mrs x3, sp_el0
    ldr x0, [x3, 0]
    ldr x1, [x3, 8]
    eret

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    ret