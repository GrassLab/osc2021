.text
.global _start

_start:
/* read cpu id, stop slave cores */
	mrs    x1, mpidr_el1 // mpidr_ell: device information
	and    x1, x1, #0xff // mask information to get cpu id
	cbz    x1, _setup_stack // if cpu id == 0 --> go to set up stack pointer

_error:  
	wfe
	b				_error


_setup_stack:
	ldr 		x0, =_stack_top
	mov 		sp, x0

_setup_bss:
	ldr     x1, =_bss_top
	ldr     w2, =bss_size

_init_bss:
	cbz     w2, _main		// if there is no bss --> go to run main
	str     xzr, [x1], #8
	sub     w2, w2, #1
	cbnz    w2, _init_bss


_main:
	bl	main // jump to C code, should not return
	b		_error // for failsafe, halt this core too

