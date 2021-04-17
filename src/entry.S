.macro vector_block label
    .align 7
    save_basic_reg
    b \label
.endm

.macro todo_vector_block num
    .align 7
    mov x0, #\num
    save_basic_reg
    b error_handler
.endm

#define context_switch_size 0x400
.macro save_basic_reg
    sub sp, sp, context_switch_size
    stp x0, x1, [sp, 0x00]
    stp x2, x3, [sp, 0x10]
    stp x4, x5, [sp, 0x20]
    stp x6, x7, [sp, 0x30]
    stp x8, x9, [sp, 0x40]
    stp x10, x11, [sp, 0x50]
    stp x12, x13, [sp, 0x60]
    stp x14, x15, [sp, 0x70]
    stp x16, x17, [sp, 0x80]
    stp x18, x19, [sp, 0x90]
    stp x20, x21, [sp, 0xa0]
    stp x22, x23, [sp, 0xb0]
    stp x24, x25, [sp, 0xc0]
    stp x26, x27, [sp, 0xd0]
    stp x28, x29, [sp, 0xf0]
    str x30, [sp, 0x100]
.endm

.macro load_basic_reg
    ldp x0, x1, [sp, 0x00]
    ldp x2, x3, [sp, 0x10]
    ldp x4, x5, [sp, 0x20]
    ldp x6, x7, [sp, 0x30]
    ldp x8, x9, [sp, 0x40]
    ldp x10, x11, [sp, 0x50]
    ldp x12, x13, [sp, 0x60]
    ldp x14, x15, [sp, 0x70]
    ldp x16, x17, [sp, 0x80]
    ldp x18, x19, [sp, 0x90]
    ldp x20, x21, [sp, 0xa0]
    ldp x22, x23, [sp, 0xb0]
    ldp x24, x25, [sp, 0xc0]
    ldp x26, x27, [sp, 0xd0]
    ldp x28, x29, [sp, 0xf0]
    ldr x30, [sp, 0x100]
    add sp, sp, context_switch_size
.endm

.macro save_kernel_reg
    save_basic_reg
    ldr lr, [sp, 0x110]
.endm

.macro load_kernel_reg
    str lr, [sp, 0x110]
    load_basic_reg
.endm

.align 11
.global exception_vector_table
exception_vector_table:
/* part 0 */
    todo_vector_block 0
    todo_vector_block 1
    todo_vector_block 2
    todo_vector_block 3
/* part 1 */
    todo_vector_block 4
    todo_vector_block 5
    todo_vector_block 6
    todo_vector_block 7
/* part 2 */
    vector_block aarch64_sync
    vector_block aarch64_irq
    todo_vector_block 10
    todo_vector_block 11
/* part 3 */
    todo_vector_block 12
    todo_vector_block 13
    todo_vector_block 14
    todo_vector_block 15


sp_el0_sync:
sp_el0_irq:
sp_el0_fiq:
sp_el0_vserror:
sp_elx_sync:
sp_elx_irq:
sp_elx_fiq:
sp_elx_vserror:

#define svc_mask 0xfffff
aarch64_sync:
    mrs x10, elr_el1
    sub x10, x10, #4
    ldr x10, [x10]
    ldr x11, =svc_mask
    and x10, x10, x11
    lsr x10, x10, #5    // svc imm
    cmp x10, #0x80
    bne 1f
    cmp x19, #60 // system call: exit
    beq process_exit
    bl sys_call_handler
    load_basic_reg
    eret
1:
    mov x0, #8
    b error_handler
process_exit:
    load_basic_reg /* drop process context */
    load_basic_reg
    ret

aarch64_irq:
    b irq_lower_el
aarch64_fiq:
aarch64_vserror:
aarch32_sync:
aarch32_irq:
aarch32_fiq:
aarch32_vserror:

error_handler:
    bl print_exc_error
1:
    b 1b

irq_lower_el:
    bl disable_DAIF
    mrs x0, cntfrq_el0
    add x0, x0, x0
    msr cntp_tval_el0, x0
    bl kprint_time
    load_basic_reg
    eret
    b irq_lower_el

.global from_el1_to_el0
from_el1_to_el0:
    save_basic_reg
    msr elr_el1, x0
    //mov x0, 0x340
    mov x0, 0x3c0
    msr spsr_el1, x0
    mov x0, #0x70000
    msr sp_el0, x0
    bl enable_irq_system_timer1
    bl enable_DAIF_irq
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

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1] // unmask timer interrupt
    mov x0, 0x340
    msr spsr_el1, x0
    ret
