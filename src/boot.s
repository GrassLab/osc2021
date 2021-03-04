.section ".text.boot"

.global _start

_start:
	mrs		x1, MPIDR_EL1
	and		x1, x1, #3
	cbz 	x1, 2f

1:
	wfe
	b			1b

2:
	ldr    x1, = _start
	mov    sp, x1

	//clear bss
	ldr    x1, = _bss_start
	ldr	   x2, = _bss_size
3:	cbz	   x2, 4f
	str    xzr, [x1], #8
	sub    x2, x2, #1
	cbnz   x2, 3b


4:  bl    main
	b     1b  
