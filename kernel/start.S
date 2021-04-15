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
    bl from_el2_to_el1
    bl set_exception_vector_table
    ldr x0, = bss_begin
    ldr x1, = bss_end
    sub x1, x1, x0
    cbz x1, 4f //if bss size = 0, no need to clear bss, goto 4

memzero: //clear the bss
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero

4:
    ldr x0, = __stack_top
    mov sp, x0 //set stack
    bl main //jump to main.c
    b 1b

