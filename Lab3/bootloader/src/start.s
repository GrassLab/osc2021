.section ".text"

.global _start

_start:
	
	mrs	x1, MPIDR_EL1
	and	x1, x1, #3
	cbz	x1, 2f

1:
	wfe
	b	1b


2:	ldr	x1, =__bss_start
	ldr	x2, =__bss_size
	cbz	x2, 3f
	str	xzr, [x1], #8
	sub	x2, x2, #1
	cbnz	x2, 2b

3:	ldr x1, =0x80000
	ldr x2, =0x60000
	ldr x3, =__load_size
	cbz x3, go_address
	
4:	ldr	x4, [x1], #8
	str	x4, [x2], #8
	sub	x3, x3, #1
	cbnz	x3, 4b
	b go_address
	
init:
    ldr x0, = 0x00080000
    ldr x1, = __bss_size
    ldr x2, = main 
    subs x2, x2, #0x00080000
    add x2, x2, #0x00060000
    cbz x1, goto_main

clear:
    str xzr, [x0], #8
    subs x1, x1, #1
    cbnz  x1, clear
    b goto_main

go_address:
    ldr x0, = 0xA0000
    mov sp, x0
    ldr x0, = init
    subs x0, x0, #0x00080000 
    add x0, x0, #0x00060000 
    br x0

goto_main:
    br x2 
    b 1b

