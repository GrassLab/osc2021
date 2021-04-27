	.arch armv8-a
	.file	"fork_test.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"Fork Test, pid %d\n"
	.align	3
.LC1:
	.string	"pid: %d, cnt: %d, ptr: %x\n"
	.align	3
.LC2:
	.string	"parent here, pid %d, child %d\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -32]!
	add	x29, sp, 0
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	mov	w0, 1
	str	w0, [x29, 24]
	str	wzr, [x29, 28]
	bl	fork
	str	w0, [x29, 28]
	ldr	w0, [x29, 28]
	cmp	w0, 0
	bne	.L2
	bl	getpid
	mov	w4, w0
	ldr	w1, [x29, 24]
	add	x2, x29, 24
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	x3, x2
	mov	w2, w1
	mov	w1, w4
	bl	uart_printf
	ldr	w0, [x29, 24]
	add	w0, w0, 1
	str	w0, [x29, 24]
	bl	fork
	b	.L3
.L4:
	bl	getpid
	mov	w4, w0
	ldr	w1, [x29, 24]
	add	x2, x29, 24
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	mov	x3, x2
	mov	w2, w1
	mov	w1, w4
	bl	uart_printf
	mov	w0, 16960
	movk	w0, 0xf, lsl 16
	bl	delay
	ldr	w0, [x29, 24]
	add	w0, w0, 1
	str	w0, [x29, 24]
.L3:
	ldr	w0, [x29, 24]
	cmp	w0, 4
	ble	.L4
	b	.L5
.L2:
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	ldr	w2, [x29, 28]
	bl	uart_printf
.L5:
	bl	exit
	mov	w0, 0
	ldp	x29, x30, [sp], 32
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
