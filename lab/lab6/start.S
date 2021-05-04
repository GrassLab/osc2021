.section ".text"
.global _start

_start:

_setup_bss:
	ldr     x5, =_bss_top
	ldr     w2, =bss_size

_init_bss:
	cbz     w2, _main		// if there is no bss --> go to run main
	str     xzr, [x5], #8 // zxr is a register always contain 0
	sub     w2, w2, #1
	cbnz    w2, _init_bss

_main:
	bl 	main // jump to C code, should not return
	bl 	exit
_hang:
	b _hang







