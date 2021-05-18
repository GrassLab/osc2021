#define context_switch_size 0x400
.macro vector_block label
    save_basic_reg
    b \label
.endm

.macro kvector_block label
    save_kernel_reg
    b \label
.endm

.macro todo_vector_block num
    save_kernel_reg
    mov x0, #\num
    b error_handler
.endm

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
    stp x28, x29, [sp, 0xe0]
    str x30, [sp, 0xf0]
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
    ldp x28, x29, [sp, 0xe0]
    ldr x30, [sp, 0xf0]
    add sp, sp, context_switch_size
.endm

.macro save_kernel_reg
    save_basic_reg
    mrs x0, sp_el0
    mrs x1, elr_el1
    stp x0, x1, [sp, 0x100]
.endm

.macro load_kernel_reg
    ldp x0, x1, [sp, 0x100]
    msr sp_el0, x0
    msr elr_el1, x1
    load_basic_reg
.endm

.align 11
.global exception_vector_table
exception_vector_table:
/* part 0 */
    .align 7
    todo_vector_block 0
    .align 7
    todo_vector_block 1
    .align 7
    todo_vector_block 2
    .align 7
    todo_vector_block 3
/* part 1 */
    .align 7
    todo_vector_block 4
    .align 7
    kvector_block sp_elx_irq
    .align 7
    todo_vector_block 6
    .align 7
    todo_vector_block 7
/* part 2 */
    .align 7
    kvector_block aarch64_sync
    .align 7
    save_kernel_reg
    b aarch64_irq
    .align 7
    todo_vector_block 10
    .align 7
    todo_vector_block 11
/* part 3 */
    .align 7
    todo_vector_block 12
    .align 7
    todo_vector_block 13
    .align 7
    todo_vector_block 14
    .align 7
    todo_vector_block 15


sp_el0_sync:
sp_el0_irq:
sp_el0_fiq:
sp_el0_vserror:
sp_elx_sync:
sp_elx_irq:
    bl sp_elx_irq_handler
    load_kernel_reg
    eret

sp_elx_fiq:
sp_elx_vserror:

#define svc_mask 0xfffff
.global uart_sendi
aarch64_sync:
    //bl disable_DAIF
    mrs x0, esr_el1
    ldr x1, =svc_mask
    and x0, x0, x1
    cmp x0, #0x80
    bne 1f
    mov x0, sp
    bl sys_call_handler
.global new_task_entry
new_task_entry:
    //bl enable_DAIF_irq
    load_kernel_reg
    eret
1:
    mov x0, #8
    b exc_error
process_exit:
    load_basic_reg /* drop process context */
    load_basic_reg
    ret

aarch64_irq:
    bl aarch64_irq_handler
    load_kernel_reg
    eret
aarch64_fiq:
aarch64_vserror:
aarch32_sync:
aarch32_irq:
aarch32_fiq:
aarch32_vserror:

error_handler:
    mov x1, sp
    bl exc_error
1:
    b 1b


.global from_el1_to_el0
from_el1_to_el0:
    save_basic_reg
    msr elr_el1, x0 // program start address
    mov x0, 0x340
    msr spsr_el1, x0
    bl core_timer_enable
    eret

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
