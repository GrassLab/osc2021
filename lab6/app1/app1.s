	.arch armv8-a
	.file	"app1.c"
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
	.align	3
.LC3:
	.string	"app2"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
.LFB0:
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	w0, [sp, 28]
	str	x1, [sp, 16]
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	str	wzr, [sp, 60]
	b	.L2
.L3:
	ldrsw	x0, [sp, 60]
	lsl	x0, x0, 3
	ldr	x1, [sp, 16]
	add	x0, x1, x0
	ldr	x0, [x0]
	mov	x1, x0
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	ldr	w0, [sp, 60]
	add	w0, w0, 1
	str	w0, [sp, 60]
.L2:
	ldr	w1, [sp, 60]
	ldr	w0, [sp, 28]
	cmp	w1, w0
	blt	.L3
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	str	x0, [sp, 40]
	str	xzr, [sp, 48]
	add	x0, sp, 40
	mov	x1, x0
	adrp	x0, .LC3
	add	x0, x0, :lo12:.LC3
	bl	exec
	bl	cur_exit
	mov	w0, 0
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
