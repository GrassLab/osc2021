.section ".text"
.global _start

_start:
// _setup_stack:
	ldr 		x0, =_stack_top
	mov 		sp, x0
	// mov sp, #0x20000000

_setup_bss:
	ldr     x1, =_bss_top
	ldr     w2, =bss_size

_init_bss:
	cbz     w2, _main		// if there is no bss --> go to run main
	str     xzr, [x1], #8 // zxr is a register always contain 0
	sub     w2, w2, #8
	cbnz    w2, _init_bss

_main:
	bl	main // jump to C code, should not return

