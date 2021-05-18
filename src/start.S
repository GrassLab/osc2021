.section ".text"
.global _start

_start:
	mrs    x1, mpidr_el1 // read cpu id
	and    x1, x1, #0xff
	cbz    x1, 2f

1:
	wfe
	b 1b
2:
	bl from_el2_to_el1
	bl set_exception_vector_table
	ldr    x1, =__dtb_addr
	str    x0, [x1]
	ldr    x1, =__stack_top
	mov    sp, x1

	ldr    x1, =__bss_start
	ldr    w2, =__bss_size
3:	
	cbz    w2, 4f
	str    xzr, [x1], #8
	sub    w2, w2, #1
	cbnz   w2, 3b
4:  
	bl main
	b 1b

from_el2_to_el1:
	mov x0, (1 << 31)
	msr hcr_el2, x0
	mov x0, 0x3c5
	msr spsr_el2, x0
	msr elr_el2, lr 
	mov x0, #(3 << 20)
	msr cpacr_el1, x0
	eret

