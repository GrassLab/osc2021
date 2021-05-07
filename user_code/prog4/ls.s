	.arch armv8-a
	.file	"ls.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"Name: %s, size: %d\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -160]!
	add	x29, sp, 0
	str	w0, [x29, 28]
	str	x1, [x29, 16]
	ldr	x0, [x29, 16]
	add	x0, x0, 8
	ldr	x0, [x0]
	mov	w1, 0
	bl	open
	str	w0, [x29, 152]
	str	wzr, [x29, 148]
	str	wzr, [x29, 156]
.L5:
	add	x0, x29, 40
	ldr	w2, [x29, 156]
	mov	x1, x0
	ldr	w0, [x29, 152]
	bl	list_fs
	str	w0, [x29, 144]
	ldr	w0, [x29, 144]
	cmp	w0, 0
	blt	.L8
	ldr	w0, [x29, 144]
	cmp	w0, 0
	ble	.L4
	add	x1, x29, 40
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	ldr	w2, [x29, 144]
	bl	uart_printf
.L4:
	ldr	w0, [x29, 156]
	add	w0, w0, 1
	str	w0, [x29, 156]
	b	.L5
.L8:
	nop
	bl	exit
	mov	w0, 0
	ldp	x29, x30, [sp], 160
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
