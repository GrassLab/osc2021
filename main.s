	.arch armv8-a
	.file	"main.c"
	.text
	.align	2
	.p2align 3,,7
	.global	strCmp
	.type	strCmp, %function
strCmp:
.LFB0:
	.cfi_startproc
	ldrb	w3, [x0]
	sub	x4, x1, #1
	mov	x2, 1
	ldrb	w1, [x1]
	cbnz	w3, .L2
	b	.L3
	.p2align 2,,3
.L4:
	ldrb	w3, [x0, x2]
	add	x2, x2, 1
	ldrb	w1, [x4, x2]
	cbz	w3, .L6
.L2:
	cmp	w3, w1
	beq	.L4
.L3:
	sub	w0, w3, w1
	ret
	.p2align 2,,3
.L6:
	mov	w3, 0
	sub	w0, w3, w1
	ret
	.cfi_endproc
.LFE0:
	.size	strCmp, .-strCmp
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align	3
.LC0:
	.string	"Booting...\n"
	.align	3
.LC1:
	.string	"> "
	.align	3
.LC2:
	.string	"help"
	.align	3
.LC3:
	.string	"help:  print all available commands\n"
	.align	3
.LC4:
	.string	"hello:  print Hello World!\n"
	.align	3
.LC5:
	.string	"hello"
	.align	3
.LC6:
	.string	"Hello World!\n"
	.align	3
.LC7:
	.string	"reboot"
	.align	3
.LC8:
	.string	"cancel"
	.align	3
.LC9:
	.string	"Command not found\n"
	.section	.text.startup,"ax",@progbits
	.align	2
	.p2align 3,,7
	.global	main
	.type	main, %function
main:
.LFB1:
	.cfi_startproc
	stp	x29, x30, [sp, -160]!
	.cfi_def_cfa_offset 160
	.cfi_offset 29, -160
	.cfi_offset 30, -152
	mov	x29, sp
	stp	x19, x20, [sp, 16]
	.cfi_offset 19, -144
	.cfi_offset 20, -136
	adrp	x19, .LC7
	adrp	x20, .LC8
	add	x19, x19, :lo12:.LC7
	add	x20, x20, :lo12:.LC8
	stp	x21, x22, [sp, 32]
	.cfi_offset 21, -128
	.cfi_offset 22, -120
	adrp	x21, .LC1
	add	x21, x21, :lo12:.LC1
	stp	x27, x28, [sp, 80]
	.cfi_offset 27, -80
	.cfi_offset 28, -72
	add	x28, sp, 104
	adrp	x22, .LC2
	adrp	x27, .LC9
	stp	x23, x24, [sp, 48]
	.cfi_offset 23, -112
	.cfi_offset 24, -104
	adrp	x24, .LC3
	adrp	x23, .LC4
	stp	x25, x26, [sp, 64]
	.cfi_offset 25, -96
	.cfi_offset 26, -88
	adrp	x25, .LC5
	adrp	x26, .LC6
	bl	uart_init
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	bl	uart_puts
	.p2align 3,,7
.L10:
	mov	x0, x21
	bl	uart_puts
	mov	x0, x28
	mov	w1, 50
	bl	uart_gets
	ldrb	w5, [sp, 104]
	add	x4, x22, :lo12:.LC2
	mov	x0, 0
	mov	w2, 104
	mov	w1, w5
	cbnz	w5, .L11
	b	.L12
	.p2align 2,,3
.L14:
	ldrb	w1, [x3, 1]
	ldrb	w2, [x0, x4]
	cbz	w1, .L13
.L11:
	add	x3, x28, x0
	cmp	w1, w2
	add	x0, x0, 1
	beq	.L14
.L40:
	mov	w1, w5
	add	x4, x25, :lo12:.LC5
	mov	x0, 0
	mov	w2, 104
	b	.L17
	.p2align 2,,3
.L19:
	ldrb	w1, [x3, 1]
	ldrb	w2, [x0, x4]
	cbz	w1, .L43
.L17:
	add	x3, x28, x0
	cmp	w1, w2
	add	x0, x0, 1
	beq	.L19
.L41:
	mov	w1, w5
	mov	x0, 0
	mov	w2, 114
	b	.L21
	.p2align 2,,3
.L23:
	ldrb	w1, [x3, 1]
	ldrb	w2, [x0, x19]
	cbz	w1, .L37
.L21:
	add	x3, x28, x0
	cmp	w2, w1
	add	x0, x0, 1
	beq	.L23
.L42:
	mov	x0, 0
	mov	w1, 99
	b	.L25
	.p2align 2,,3
.L27:
	ldrb	w5, [x2, 1]
	ldrb	w1, [x0, x20]
	cbz	w5, .L38
.L25:
	add	x2, x28, x0
	cmp	w1, w5
	add	x0, x0, 1
	beq	.L27
.L12:
	add	x0, x27, :lo12:.LC9
	bl	uart_puts
	b	.L10
	.p2align 2,,3
.L13:
	cbnz	w2, .L40
	add	x0, x24, :lo12:.LC3
	bl	uart_puts
	add	x0, x23, :lo12:.LC4
	bl	uart_puts
	b	.L10
	.p2align 2,,3
.L43:
	cbnz	w2, .L41
	add	x0, x26, :lo12:.LC6
	bl	uart_puts
	b	.L10
	.p2align 2,,3
.L37:
	cbnz	w2, .L42
	mov	w0, 10
	bl	reset
	b	.L10
.L38:
	cbnz	w1, .L12
	bl	cancel_reset
	b	.L10
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
