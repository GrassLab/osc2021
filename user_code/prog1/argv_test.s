	.arch armv8-a
	.file	"argv_test.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"Argv Test, pid %d\n"
	.align	3
.LC1:
	.string	"%s\n"
	.align	3
.LC2:
	.string	"fork_test"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	stp	x29, x30, [sp, -64]!
	add	x29, sp, 0
	str	w0, [x29, 28]
	str	x1, [x29, 16]
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	str	wzr, [x29, 60]
	b	.L2
.L3:
	ldrsw	x0, [x29, 60]
	lsl	x0, x0, 3
	ldr	x1, [x29, 16]
	add	x0, x1, x0
	ldr	x1, [x0]
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	ldr	w0, [x29, 60]
	add	w0, w0, 1
	str	w0, [x29, 60]
.L2:
	ldr	w1, [x29, 60]
	ldr	w0, [x29, 28]
	cmp	w1, w0
	blt	.L3
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	str	x0, [x29, 40]
	str	xzr, [x29, 48]
	add	x1, x29, 40
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	exec
	mov	w0, 0
	ldp	x29, x30, [sp], 64
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
