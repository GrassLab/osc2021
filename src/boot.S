.section ".text.boot"

.global _start

_start:
		bl    from_el2_to_el1
		mrs		x1, MPIDR_EL1
		and		x1, x1, #3
		cbz 	x1, 2f

1:
		wfe
		b			1b

2:
		bl     set_exception_vector_table
		bl     core_timer_enable
		bl		 set_mmu
		ldr  	 x2, =_virtual_start
		br		 x2

_virtual_start:
		ldr    x1, = _start
		mov    sp, x1

		//clear bss
		ldr    x1, = _bss_start
		ldr	   x2, = _bss_size
3:	cbz	   x2, 4f
		str    xzr, [x1], #8
		sub    x2, x2, #1
		cbnz   x2, 3b


4:	bl    kernel_main
		b     1b
