// Some compiler do not provide alias for special registers
// So we defined it here
fp .req x29
lr .req x30


// EL1 based thread switching
// Store old thread context to memory and load the new one
//
// Ref: ARM Procedure Call Standard(PCS)
//  Corruptible Registers: X9-x15 (Use it without cares)
//  Callee saved Registers: x19-x23, x30
//  We have saved those registers in the caller stack before calling
//
// signature: void switch_to(struct task_struct *prev, struct task_struct *next);
.global switch_to
switch_to:
    // prev == NULL
    cmp x0, #0
    beq __switch_load

    // prev == next
    cmp x0, x1
    beq __switch_load

__switch_save:
    // x0: struct task_struct  *prev
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5] // x29(fp), x30(lr)
    mov x9, sp
    str x9, [x0, 16 * 6]

__switch_load:
    cmp x1, #0
    beq __switch_ret
    // x1: struct task_struct  *next
    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp fp, lr, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9

__switch_ret:
    // Store pointer to currnet thread in tpidr_el1
    msr tpidr_el1, x1
    ret