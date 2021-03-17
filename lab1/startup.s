.section ".text.boot"

.global _start

_start:
	mrs    x1, mpidr_el1
	and    x1, x1, #3
	cbz    x1, init
	b 	   busy_loop

busy_loop: 	
	wfe
	b 	   busy_loop

init: 	
	ldr    x1, =__bss_start
	ldr    x2, =__bss_size

loop_clear_bss:
	cbz    x2, entry_point
	str    xzr, [x1], #8
	sub    x2, x2, #1
	cbnz   x2, loop_clear_bss

entry_point:
	ldr    x1, =_start
	mov    sp, x1
	bl	   main
	b 	   busy_loop

