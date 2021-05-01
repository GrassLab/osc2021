.section ".text.boot"

.global _start

_start:

1:  
	wfe
    b       1b

2:    
    // set stack before our code
    ldr x0, = _stack_top
    mov sp, x0

    // clear bss
    ldr     x0, =__bss_start
    ldr     w2, =__bss_size
3:      
    // jump to C code, should not return
    bl main
    
    // for failsafe, halt this core too
    b 1b
    