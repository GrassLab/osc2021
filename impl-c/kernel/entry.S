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

.section ".text.boot"

// create alias for lr(link register) x30
lr .req x30

.global _start

_start:
    // Get cpuid
    mrs x0, MPIDR_EL1
    and x0, x0, #3 // Get the first two bit in Aff0 fields
    cbz x0, 2f     // only cpuid=0 could go to label 2

// Busy Loop
1:
    wfe
    b 1b

// cpuid = 0
2:
    // Set exception level to el1
    bl from_el2_to_el1

    // Set exception table
    ldr x0, =exception_vector_table
    msr vbar_el1, x0

    // Initialize stack pointer: set to __stack_top
    ldr x0, = __stack_top
    mov sp, x0

    // Clear bss
    ldr     x0, =__bss_start
    ldr     x1, =__bss_size
3:  cbz     x1, 4f          // If size(bss)=0, bss is cleared
    str     xzr, [x0], #8   //  *x0 = zero, x0 = x0 + 8
    sub     x1, x1, #1      //  x1 = x1 - 1
    cbnz    x1, 3b          // Loop back to label 3 until bss is cleared

4:  bl      main  // Jump to Kernel Entry Point (Main function)
    b       1b    // Stay in busy loop if returned


from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr

    // Enable access to SIMD/Floating point registers
    mov x0, #(3 << 20)
    msr cpacr_el1, x0

    eret // return to EL1


not_impl:
    save_all
    bl _handler_not_impl
    load_all
    eret

_syn_handler:
    save_all
    mov x0, sp
    bl syn_handler
    load_all
    eret

_irq_handler:
    save_all
    bl irq_handler
    load_all
    eret


.align 11
.global exception_vector_table
exception_vector_table:

    // Exception from current EL with SP_EL0
    b not_impl   //  Synchronous
    .align 7
    b not_impl   //  IRQ
    .align 7
    b not_impl   //  FIQ
    .align 7
    b not_impl   //  SError
    .align 7

    // Exception from current EL with SP_ELx
    b _syn_handler  //  Synchronous
    .align 7
    b not_impl   //  IRQ
    .align 7
    b not_impl   //  FIQ
    .align 7
    b not_impl   //  SError
    .align 7

    // Exception from lower EL/aarch64
    b _syn_handler   // Synchronous
    .align 7
    b _irq_handler   //  IRQ
    .align 7
    b not_impl   //  FIQ
    .align 7
    b not_impl   //  SError
    .align 7

    // Exception from lower EL/aarch32
    b not_impl   //  Synchronous
    .align 7
    b not_impl   //  IRQ
    .align 7
    b not_impl   //  FIQ
    .align 7
    b not_impl   //  SError
    .align 7
