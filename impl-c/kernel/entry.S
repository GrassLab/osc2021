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
    // Set exception levle to el1
    bl from_el2_to_el1

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
    eret // return to EL1
