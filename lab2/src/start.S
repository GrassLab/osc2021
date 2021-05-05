.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
    // cpu id > 0, stop
1:  
    wfe
    b 1b
2:  
    // set stack before our code
    ldr x1, =_start
    mov sp, x1

    // clear bss
    ldr x1, =__bss_start
    ldr x2, =__bss_size

clear_bss:
    cbz x2, 4f

    // post-indexed store
    str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, clear_bss

4:  
    b main