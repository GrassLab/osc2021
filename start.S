.section = ".text"

.global _start

_start:
    mrs x0, mpidr_el1 //get CPU id
    and x0, x0, #3 
    cbz x0, 2f //if CPU id == 0, goto 2

//infinite loop for CPU id != 0
1:
    wfe
    b 1b

2:
    ldr x0, = bss_begin
    ldr x1, = bss_end
    sub x1, x1, x0
    bl memzero

memzero: //clear the bss
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero

4:
    ldr x0, = _start
    mov sp, x0 //set stack
    bl main //jump to main.c
    b 1b
