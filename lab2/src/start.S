.section ".text.boot"

.globl _start

_start:
	mrs    x0, mpidr_el1
    and    x0, x0, #3 
	cbz    x0, _kernel_loader

_proc_hang:
	wfe
	b _proc_hang

_kernel_loader:
	ldr x0, =_start
	mov sp, x0
	ldr x0, =_bss_begin
	ldr x1, =_bss_end
_bss_init:
	str xzr, [x0], #8
	cmp x0, x1
	bne _bss_init

_load_main:
	bl main
       
	
