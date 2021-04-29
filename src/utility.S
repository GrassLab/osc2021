.extern boot_info

.global get_spsr_el1
get_spsr_el1:
    mrs x0, spsr_el1
    ret

.global get_elr_el1
get_elr_el1:
    mrs x0, elr_el1
    ret

.global get_esr_el1
get_esr_el1:
    mrs x0, esr_el1
    ret

.global time_freq
time_freq:
    mrs x0, CNTFRQ_EL0
    ret

.global time_counter
time_counter:
    mrs x0, CNTPCT_EL0
    ret

.global set_cntp_tval_el0
set_cntp_tval_el0:
    msr cntp_tval_el0, x0
    ret

.global set_cntp_ctl_el0
set_cntp_ctl_el0:
    msr cntp_ctl_el0, x0
    ret

.global set_spsr_el1
set_spsr_el1:
    msr spsr_el1, x0
    ret

.global get__start
get__start:
    ldr x0, =_start
    ret

.global jump_address
jump_address:
    mov x20, x0
    ldr x0, =boot_info
    br x20

.global get_x0
get_x0:
    ret

.global get_x19
get_x19:
    mov x0, x19
    ret

.global get_x30
get_x30:
    mov x0, x30
    ret

/*
 * PSTATE.{D, A, I, F}
 * D: Debug exception mask bit
 * A: SError interrupt mask bit
 * I: IRQ interrupt mask bit
 * F: FIQ interrupt mask bit
 */
.global enable_DAIF_fiq
enable_DAIF_fiq:
    msr DAIFClr, #1
    ret
.global enable_DAIF_irq
enable_DAIF_irq:
    msr DAIFClr, #2
    ret
.global enable_DAIF_serror
enable_DAIF_serror:
    msr DAIFClr, #4
    ret
.global enable_DAIF_debug
enable_DAIF_debug:
    msr DAIFClr, #8
    ret
.global enable_DAIF
enable_DAIF:
    msr DAIFClr, #0xf
    ret

.global disable_DAIF_fiq
disable_DAIF_fiq:
    msr DAIFSet, #1
    ret
.global disable_DAIF_irq
disable_DAIF_irq:
    msr DAIFSet, #2
    ret
.global disable_DAIF_serror
disable_DAIF_serror:
    msr DAIFSet, #4
    ret
.global disable_DAIF_debug
disable_DAIF_debug:
    msr DAIFSet, #8
    ret
.global disable_DAIF
disable_DAIF:
    msr DAIFSet, #0xf
    ret

