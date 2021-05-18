	.arch armv8-a
	.file	"test.c"
	.text
	.section	.text.startup,"ax",@progbits
	.align	2
	.p2align 3,,7
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -48]!
	add	x29, sp, 0
	str	x19, [sp, 16]
	bl	_uart_getc
	and	x0, x0, 255
	add	x19, x29, 48
	add	x0, x0, 1
	str	x0, [x29, 40]
	bl	_uart_getc
	and	x0, x0, 255
	add	x0, x0, 1
	str	x0, [x19, -8]!
	bl	print_hex
	mov	w0, w19
	bl	print_hex
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 48
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
