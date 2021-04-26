	.arch armv8-a
	.file	"app2.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"read test: \n"
	.align	3
.LC1:
	.string	"%s,len: %d\n"
	.align	3
.LC2:
	.string	"Fork Test, pid %d\n"
	.align	3
.LC3:
	.string	"pid: %d, cnt: %d, ptr: %x\n"
	.align	3
.LC4:
	.string	"parent here, pid %d, child %d, ptr: %x\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	sub	sp, sp, #544
	stp	x29, x30, [sp]
	add	x29, sp, 0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	add	x0, x29, 32
	mov	w1, 500
	bl	uart_read
	str	w0, [x29, 540]
	add	x1, x29, 32
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	ldr	w2, [x29, 540]
	bl	uart_printf
	add	x0, x29, 32
	mov	w1, 2
	bl	uart_write
	str	w0, [x29, 540]
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	uart_printf
	mov	w0, 1
	str	w0, [x29, 28]
	str	wzr, [x29, 536]
	bl	fork
	str	w0, [x29, 536]
	ldr	w0, [x29, 536]
	cmp	w0, 0
	bne	.L2
	bl	getpid
	mov	w4, w0
	ldr	w1, [x29, 28]
	add	x2, x29, 28
	adrp	x0, .LC3
	add	x0, x0, :lo12:.LC3
	mov	x3, x2
	mov	w2, w1
	mov	w1, w4
	bl	uart_printf
	ldr	w0, [x29, 28]
	add	w0, w0, 1
	str	w0, [x29, 28]
	bl	fork
	b	.L3
.L4:
	bl	getpid
	mov	w4, w0
	ldr	w1, [x29, 28]
	add	x2, x29, 28
	adrp	x0, .LC3
	add	x0, x0, :lo12:.LC3
	mov	x3, x2
	mov	w2, w1
	mov	w1, w4
	bl	uart_printf
	mov	w0, 16960
	movk	w0, 0xf, lsl 16
	bl	delay
	ldr	w0, [x29, 28]
	add	w0, w0, 1
	str	w0, [x29, 28]
.L3:
	ldr	w0, [x29, 28]
	cmp	w0, 4
	ble	.L4
	b	.L5
.L2:
	bl	getpid
	mov	w4, w0
	add	x1, x29, 28
	adrp	x0, .LC4
	add	x0, x0, :lo12:.LC4
	mov	x3, x1
	ldr	w2, [x29, 536]
	mov	w1, w4
	bl	uart_printf
.L5:
	bl	exit
	.size	main, .-main
	.ident	"GCC: (Linaro GCC 7.5-2019.12) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
