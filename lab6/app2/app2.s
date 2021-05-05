	.arch armv8-a
	.file	"app2.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"Fork Test, pid:%d\n"
	.align	3
.LC1:
	.string	"pid:%d, cnt:%d, ptr:%x\n"
	.align	3
.LC2:
	.string	"parent here, pid: %d\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
.LFB0:
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	str	w0, [sp, 28]
	str	x1, [sp, 16]
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_printf
	str	wzr, [sp, 40]
	mov	w0, 1
	str	w0, [sp, 36]
	bl	sys_fork
	str	w0, [sp, 40]
	ldr	w0, [sp, 40]
	cmp	w0, 0
	bne	.L2
	bl	getpid
	mov	w4, w0
	ldr	w0, [sp, 36]
	add	x1, sp, 36
	mov	x3, x1
	mov	w2, w0
	mov	w1, w4
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	ldr	w0, [sp, 36]
	add	w0, w0, 1
	str	w0, [sp, 36]
	bl	sys_fork
	b	.L3
.L6:
	bl	getpid
	mov	w4, w0
	ldr	w0, [sp, 36]
	add	x1, sp, 36
	mov	x3, x1
	mov	w2, w0
	mov	w1, w4
	adrp	x0, .LC1
	add	x0, x0, :lo12:.LC1
	bl	uart_printf
	str	wzr, [sp, 44]
	b	.L4
.L5:
	ldr	w0, [sp, 44]
	add	w0, w0, 1
	str	w0, [sp, 44]
.L4:
	ldr	w1, [sp, 44]
	mov	w0, 134217727
	cmp	w1, w0
	ble	.L5
	ldr	w0, [sp, 36]
	add	w0, w0, 1
	str	w0, [sp, 36]
.L3:
	ldr	w0, [sp, 36]
	cmp	w0, 4
	ble	.L6
	b	.L7
.L2:
	bl	getpid
	mov	w1, w0
	adrp	x0, .LC2
	add	x0, x0, :lo12:.LC2
	bl	uart_printf
.L7:
	bl	cur_exit
	mov	w0, 0
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
