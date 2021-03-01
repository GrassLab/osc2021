.section ".text.boot"

.global _start
_start:
	
	ldr x1, =_bss_start
	ldr x2, =_bss_end
	b clean_bss

set_zero:
	str xzr, [x1]
	add x1, x1, #8

clean_bss:
	# zero out bss
	cmp x1, x2
	blt set_zero
	
	mov sp, #0x80000
	bl main

1:
	b 1b
