.section ".text.boot"

.global _start

_start:
    mrs    x1, MPIDR_EL1
    and    x1, x1, #3
    cbz    x1, 2f
1:
    wfe
    b      1b
2:
    //set stack pointer
    ldr    x1, =_start
    mov    sp, x1
    ldr    x1, =__bss_start 
    ldr    x2, =__bss_size
3:  
    cbz    w2, 4f
    str    xzr, [x1],#8
    sub    w2, w2, #1
    b      3b
4:
    bl     main
    b      1b
     
