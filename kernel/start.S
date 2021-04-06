.section ".text"

.global _start

_start:
    ldr x1, = _bss_start
    ldr w2, = _bss_size

// clean bss segment
3:  
    cbz w2, 2f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b

// set stack pointer and branch to main function.
2:
    ldr x0, = _start
    mov sp, x0
    bl main

1:
    b 1b
