.globl put32
put32:
    str w1,[x0]
    ret

.globl get32
get32:
    ldr w0,[x0]
    ret

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.globl branch_to_address
branch_to_address:
    br x1

.globl lead_zero
lead_zero:
    clz w0, w0
    ret

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

.globl get_sctlr_el1
get_sctlr_el1:
    mrs x0, sctlr_el1
    ret

.globl get_hcr_el2
get_hcr_el2:
    mrs x0, hcr_el2
    ret

.globl get_scr_el3
get_scr_el3:
    mrs x0, scr_el3
    ret

.globl get_spsr_el2
get_spsr_el2:
    mrs x0, spsr_el2
    ret

.globl get_cntfrq_el0
get_cntfrq_el0:
    mrs x0, cntfrq_el0
    ret

.globl run_user_program
run_user_program:
    msr    elr_el1,  x0    // user program address
    ldr    x0, =0x0        // enable irq in el0 (e.g. =0x3c0 is disable)
    msr    spsr_el1, x0    // set to EL0
    msr    sp_el0, x1      // set EL0 sp
    //bl core_timer_enable
    eret

.globl enable_irq
enable_irq:
    msr    daifclr, #2
    ret

.globl disable_irq
disable_irq:
    msr    daifset, #2
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.globl core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0  // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]          // unmask timer interrupt
    ret

.globl core_timer_disable
core_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0  // disable
    mov x0, 0
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]          // unmask timer interrupt
    ret

.globl wait_for_interrupt
wait_for_interrupt:
    wfi
    ret