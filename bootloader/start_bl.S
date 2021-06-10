.section ".text.boot"

.global _start

_start:
    mrs    x1, mpidr_el1
    and    x1, x1, #3
    cbz    x1, 2f
1:
    wfe
    b      1b

2:
    ldr    x1, =__stack_top
    mov    sp, x1
    ldr    x1, =__bss_start
    ldr    w2, =__bss_size
3:    
    cbz    w2, 4f
    str    xzr, [x1], #8
    sub    w2, w2, #1
    cbnz   w2, 3b
4:
    bl     relocate
    b      1b
