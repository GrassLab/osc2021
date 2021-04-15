#define CORE0_TIMER_IRQ_CTRL 0x40000040
.macro save_state
    sub sp, sp, 0x10 * 16
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

.macro load_state
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
    add sp, sp, 0x10 * 16
.endm

.align 11
.global exception_vector_table
exception_vector_table:
/* part 0 */
    save_state
    mov x0, #0
    b exception_handler
    .align 7

    save_state
    mov x0, #1
    b exception_handler
    .align 7

    save_state
    mov x0, #2
    b exception_handler
    .align 7

    save_state
    mov x0, #3
    b exception_handler
    .align 7

/* part 1 */
    save_state
    mov x0, #4
    b exception_handler
    .align 7

    save_state
    mov x0, #5
    b exception_handler
    .align 7

    save_state
    mov x0, #6
    b exception_handler
    .align 7

    save_state
    mov x0, #7
    b exception_handler
    .align 7

/* part 2 */
    save_state
    b sys_call
    .align 7

    msr DAIFSet, 0x2
    save_state
    mov x0, #9
    b irq_lower_el
    .align 7

    save_state
    mov x0, #10
    b exception_handler
    .align 7

    save_state
    mov x0, #11
    b exception_handler
    .align 7

/* part 3 */
    save_state
    mov x0, #12
    b exception_handler
    .align 7

    save_state
    mov x0, #13
    b exception_handler
    .align 7

    save_state
    mov x0, #14
    b exception_handler
    .align 7

    save_state
    mov x0, #15
    b exception_handler
    .align 7


exception_handler:
    b exception_handler
    load_state
    eret

irq_lower_el:
    save_state
    bl kprint_time
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0
    load_state
    msr DAIFClr, 0x2
    eret
    b irq_lower_el


/* system call handler */

sys_call:
    lsl x0, x0, #2
    ldr x1, =sys_call_table
    add x0, x0, x1
    br x0
sys_call_ret:
    load_state
    eret

sys_call_table:
    b print_el1_content
    b ret_kernel

.global print_el1_reg
print_el1_content:
    bl print_el1_reg
    b sys_call_ret

.global ret_kernel
ret_kernel:
    load_state
    ret

.global from_el1_to_el0
from_el1_to_el0:
    save_state
    msr elr_el1, x0
    mov x0, 0x3c0
    msr spsr_el1, x0
    mov x0, #0x70000
    msr sp_el0, x0
    bl core_timer_enable
    eret

.global tmp_p
tmp_p:
    mov x0, 0
1:
    add x0, x0, 1
    mov x1, x0
    mov x0, #0
    svc 0
    mov x0, x1
    cmp x0, 5
    blt 1b
1:
    mov x0, #1
    svc 0
    b 1b

#define IRQ_TMP 0x3F00B210
.global core_timer_enable
core_timer_enable:
    ldr x0, =IRQ_TMP
    mov x1, #2
    str x1, [x0]
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    mov x0, #0
    msr spsr_el1, x0
    ret
