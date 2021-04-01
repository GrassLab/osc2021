.section ".text.boot"

.global _start

_start:
    mrs x1, MPIDR_EL1
    and x1, x1, #3
    cbz x1, 2f
    //if process id > 0 will loop
1:
    //boot
    wfe 
    b 1b
2:
    //let stack point  set to a proper address
    ldr x1, =_start
    mov sp, x1

    //clean bss

    ldr x1, =__bss_start
    ldr x2, =__bss_size
    cbz x2, 4f
3:  str xzr, [x1], #8
    sub x2, x2, #1
    cbnz x2, 3b
    //loop until bss clean
    
    //go to main function
4:  bl main
    b 1b