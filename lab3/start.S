.section = ".text"

.global _start

_start:
    mrs x1, mpidr_el1 //get CPU id
    and x1, x1, #3 
    cbz x1, 2f //if CPU id == 0, goto 2

//infinite loop for CPU id != 0
1:
    wfe
    b 1b

2:
    ldr x5, =0x12345678
    ldr x1, = bss_begin
    ldr x2, = bss_end
    sub x2, x2, x1
    cbz x2, 4f
    bl memzero

memzero: //clear the bss
	str xzr, [x1], #8
	subs x2, x2, #8
	b.gt memzero

4:
    ldr x1, = __stack_top
    mov sp, x1 //set stack
    bl main //jump to main.c
    b 1b
