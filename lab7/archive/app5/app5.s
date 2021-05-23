	.arch armv8-a
	.file	"app5.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"read somthing from %s(fd %d):\n"
	.align	3
.LC1:
	.string	"%s"
	.align	3
.LC2:
	.string	"\nwrite somthing to %s(fd %d):\n"
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
	str	w0, [x29, 156]
	ldr	x0, [x29, 16]
	add	x0, x0, 8
	ldr	x1, [x0]
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	ldr	w2, [x29, 156]
	bl	uart_printf
.L4:
	add	x0, x29, 40
	mov	w2, 100
	mov	x1, x0
	ldr	w0, [x29, 156]
	bl	read
	str	w0, [x29, 152]
	ldr	w0, [x29, 152]
	cmp	w0, 0
	beq	.L6
	ldrsw	x0, [x29, 152]
	add	x1, x29, 40
	strb	wzr, [x1, x0]
	add	x1, x29, 40
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	b	.L4
.L6:
	nop
	ldr	w0, [x29, 156]
	bl	close
	ldr	x0, [x29, 16]
	add	x0, x0, 8
	ldr	x0, [x0]
	mov	w1, 0
	bl	open
	str	w0, [x29, 156]
	ldr	x0, [x29, 16]
	add	x0, x0, 8
	ldr	x1, [x0]
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	ldr	w2, [x29, 156]
	bl	uart_printf
	add	x0, x29, 40
	mov	w1, 100
	bl	uart_read
	str	w0, [x29, 148]
	add	x0, x29, 40
	ldr	w2, [x29, 148]
	mov	x1, x0
	ldr	w0, [x29, 156]
	bl	write
	bl	exit
	.size	main, .-main
	.ident	"GCC: (Linaro GCC 7.5-2019.12) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
