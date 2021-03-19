  
.section ".text.boot"

.global _start

// set stack pointer and branch to main function.
_start:
    ldr       x0, = _start
    mov       sp, x0
    // clear bss
    ldr     x0, =__bss_start
    ldr     w1, =__bss_size
    
    bl       main
1:     
    b       1b