	.arch armv8-a
	.file	"hello_world.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"in hello world\n"
	.align	3
.LC1:
	.string	"hello"
	.align	3
.LC2:
	.string	"world"
	.align	3
.LC3:
	.string	"Hello "
	.align	3
.LC4:
	.string	"World!"
	.align	3
.LC5:
	.string	"%s\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -144]!
	add	x29, sp, 0
	str	w0, [x29, 28]
	str	x1, [x29, 16]
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	w1, 1
	bl	open
	str	w0, [x29, 140]
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	mov	w1, 1
	bl	open
	str	w0, [x29, 136]
	adrp	x0, .LC3
	add	x0, x0, :lo12:.LC3
	mov	w2, 6
	mov	x1, x0
	ldr	w0, [x29, 140]
	bl	write
	adrp	x0, .LC4
	add	x0, x0, :lo12:.LC4
	mov	w2, 6
	mov	x1, x0
	ldr	w0, [x29, 136]
	bl	write
	ldr	w0, [x29, 140]
	bl	close
	ldr	w0, [x29, 136]
	bl	close
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	w1, 0
	bl	open
	str	w0, [x29, 136]
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	mov	w1, 0
	bl	open
	str	w0, [x29, 140]
	str	wzr, [x29, 132]
	add	x0, x29, 32
	mov	w2, 100
	mov	x1, x0
	ldr	w0, [x29, 136]
	bl	read
	str	w0, [x29, 132]
	ldrsw	x0, [x29, 132]
	add	x1, x29, 32
	add	x0, x1, x0
	mov	w2, 100
	mov	x1, x0
	ldr	w0, [x29, 140]
	bl	read
	mov	w1, w0
	ldr	w0, [x29, 132]
	add	w0, w0, w1
	str	w0, [x29, 132]
	ldrsw	x0, [x29, 132]
	add	x1, x29, 32
	strb	wzr, [x1, x0]
	add	x1, x29, 32
	adrp	x0, .LC5
	add	x0, x0, :lo12:.LC5
	bl	uart_printf
	bl	exit
	mov	w0, 0
	ldp	x29, x30, [sp], 144
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
