.section ".text.boot"

.global _start

_start:
    // get cpu id
    mrs x1, mpidr_el1
    and x1, x1, #0xff
    // only the primary cpu (id = 0) goes to master function
    cbz x1, master
    b hang_loop

master:
    // save dtb loading address
    //ldr x1, =0x9000000
    //str x0, [x1]

    bl from_el2_to_el1
    bl set_exception_vector_table

    // clear bss segment
    ldr x0, =__bss_start
    ldr x1, =__bss_size

clear_bss_start:
    cbz x1, clear_bss_done  // if remaining bss size == 0
    str xzr, [x0], #8       // *x0 = 0 (xzr register stores 0), x0 += 8
    sub x1, x1, #1
    cbnz x1, clear_bss_start

clear_bss_done:
    // set stack pointer
    ldr x1, =_start
    mov sp, x1

    bl main                 // jump to kernel main (c code)
    b hang_loop             // jump to endless loop if main returns (for fail-safe)

from_el2_to_el1:
    mov x0, (1 << 31)       // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5           // EL1h with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret                    // return to EL1

set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret

hang_loop:
    wfe
    b hang_loop
