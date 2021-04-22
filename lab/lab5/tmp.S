.extern print_h
.macro test
	sub sp, sp, 32
	str lr, [sp]
	str x0, [sp, 32]
	mov x0, 0xFFF000
	bl print_h
	ldr x0, [sp, 32]
	ldr lr, [sp] 
	add sp, sp, 32
.endm