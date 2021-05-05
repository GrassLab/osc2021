	.arch armv8-a
	.file	"printf.c"
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"(null)"
	.text
	.align	2
	.global	vsprintf
	.type	vsprintf, %function
vsprintf:
.LFB0:
	.cfi_startproc
	str	x19, [sp, -96]!
	.cfi_def_cfa_offset 96
	.cfi_offset 19, -96
	str	x0, [sp, 24]
	str	x1, [sp, 16]
	mov	x19, x2
	ldr	x0, [sp, 24]
	str	x0, [sp, 56]
	ldr	x0, [sp, 24]
	cmp	x0, 0
	beq	.L2
	ldr	x0, [sp, 16]
	cmp	x0, 0
	bne	.L3
.L2:
	mov	w0, 0
	b	.L52
.L3:
	str	xzr, [sp, 88]
	b	.L5
.L51:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 37
	bne	.L53
	ldr	x0, [sp, 16]
	add	x0, x0, 1
	str	x0, [sp, 16]
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 37
	beq	.L54
	str	wzr, [sp, 84]
	b	.L8
.L10:
	ldr	w1, [sp, 84]
	mov	w0, w1
	lsl	w0, w0, 2
	add	w0, w0, w1
	lsl	w0, w0, 1
	str	w0, [sp, 84]
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	sub	w0, w0, #48
	ldr	w1, [sp, 84]
	add	w0, w1, w0
	str	w0, [sp, 84]
	ldr	x0, [sp, 16]
	add	x0, x0, 1
	str	x0, [sp, 16]
.L8:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 47
	bls	.L9
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 57
	bls	.L10
.L9:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 108
	bne	.L11
	ldr	x0, [sp, 16]
	add	x0, x0, 1
	str	x0, [sp, 16]
.L11:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 99
	bne	.L12
	ldr	w1, [x19, 24]
	ldr	x0, [x19]
	cmp	w1, 0
	blt	.L13
	add	x1, x0, 11
	and	x1, x1, -8
	str	x1, [x19]
	b	.L14
.L13:
	add	w2, w1, 8
	str	w2, [x19, 24]
	ldr	w2, [x19, 24]
	cmp	w2, 0
	ble	.L15
	add	x1, x0, 11
	and	x1, x1, -8
	str	x1, [x19]
	b	.L14
.L15:
	ldr	x2, [x19, 8]
	sxtw	x0, w1
	add	x0, x2, x0
.L14:
	ldr	w0, [x0]
	sxtw	x0, w0
	str	x0, [sp, 88]
	ldr	x0, [sp, 24]
	add	x1, x0, 1
	str	x1, [sp, 24]
	ldr	x1, [sp, 88]
	and	w1, w1, 255
	strb	w1, [x0]
	ldr	x0, [sp, 16]
	add	x0, x0, 1
	str	x0, [sp, 16]
	b	.L5
.L12:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 100
	bne	.L17
	ldr	w1, [x19, 24]
	ldr	x0, [x19]
	cmp	w1, 0
	blt	.L18
	add	x1, x0, 11
	and	x1, x1, -8
	str	x1, [x19]
	b	.L19
.L18:
	add	w2, w1, 8
	str	w2, [x19, 24]
	ldr	w2, [x19, 24]
	cmp	w2, 0
	ble	.L20
	add	x1, x0, 11
	and	x1, x1, -8
	str	x1, [x19]
	b	.L19
.L20:
	ldr	x2, [x19, 8]
	sxtw	x0, w1
	add	x0, x2, x0
.L19:
	ldr	w0, [x0]
	sxtw	x0, w0
	str	x0, [sp, 88]
	str	wzr, [sp, 80]
	ldr	x0, [sp, 88]
	cmp	w0, 0
	bge	.L22
	ldr	x0, [sp, 88]
	neg	x0, x0
	str	x0, [sp, 88]
	ldr	w0, [sp, 80]
	add	w0, w0, 1
	str	w0, [sp, 80]
.L22:
	ldr	x1, [sp, 88]
	mov	x0, -2725642241
	movk	x0, 0x4578, lsl 32
	movk	x0, 0x163, lsl 48
	cmp	x1, x0
	ble	.L23
	mov	x0, -2725642241
	movk	x0, 0x4578, lsl 32
	movk	x0, 0x163, lsl 48
	str	x0, [sp, 88]
.L23:
	mov	w0, 18
	str	w0, [sp, 76]
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	strb	wzr, [x1, x0]
.L25:
	ldr	x1, [sp, 88]
	mov	x0, 7378697629483820646
	movk	x0, 0x6667, lsl 0
	smulh	x0, x1, x0
	asr	x2, x0, 2
	asr	x0, x1, 63
	sub	x2, x2, x0
	mov	x0, x2
	lsl	x0, x0, 2
	add	x0, x0, x2
	lsl	x0, x0, 1
	sub	x2, x1, x0
	and	w0, w2, 255
	ldr	w1, [sp, 76]
	sub	w1, w1, #1
	str	w1, [sp, 76]
	add	w0, w0, 48
	and	w2, w0, 255
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	strb	w2, [x1, x0]
	ldr	x0, [sp, 88]
	mov	x1, 7378697629483820646
	movk	x1, 0x6667, lsl 0
	smulh	x1, x0, x1
	asr	x1, x1, 2
	asr	x0, x0, 63
	sub	x0, x1, x0
	str	x0, [sp, 88]
	ldr	x0, [sp, 88]
	cmp	x0, 0
	beq	.L24
	ldr	w0, [sp, 76]
	cmp	w0, 0
	bgt	.L25
.L24:
	ldr	w0, [sp, 80]
	cmp	w0, 0
	beq	.L26
	ldr	w0, [sp, 76]
	sub	w0, w0, #1
	str	w0, [sp, 76]
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	mov	w2, 45
	strb	w2, [x1, x0]
.L26:
	ldr	w0, [sp, 84]
	cmp	w0, 0
	ble	.L27
	ldr	w0, [sp, 84]
	cmp	w0, 17
	bgt	.L27
	b	.L28
.L29:
	ldr	w0, [sp, 76]
	sub	w0, w0, #1
	str	w0, [sp, 76]
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	mov	w2, 32
	strb	w2, [x1, x0]
.L28:
	mov	w1, 18
	ldr	w0, [sp, 84]
	sub	w0, w1, w0
	ldr	w1, [sp, 76]
	cmp	w1, w0
	bgt	.L29
.L27:
	add	x1, sp, 32
	ldrsw	x0, [sp, 76]
	add	x0, x1, x0
	str	x0, [sp, 64]
	b	.L30
.L17:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 120
	bne	.L31
	ldr	w1, [x19, 24]
	ldr	x0, [x19]
	cmp	w1, 0
	blt	.L32
	add	x1, x0, 15
	and	x1, x1, -8
	str	x1, [x19]
	b	.L33
.L32:
	add	w2, w1, 8
	str	w2, [x19, 24]
	ldr	w2, [x19, 24]
	cmp	w2, 0
	ble	.L34
	add	x1, x0, 15
	and	x1, x1, -8
	str	x1, [x19]
	b	.L33
.L34:
	ldr	x2, [x19, 8]
	sxtw	x0, w1
	add	x0, x2, x0
.L33:
	ldr	x0, [x0]
	str	x0, [sp, 88]
	mov	w0, 16
	str	w0, [sp, 76]
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	strb	wzr, [x1, x0]
.L39:
	ldr	x0, [sp, 88]
	and	w0, w0, 255
	and	w0, w0, 15
	strb	w0, [sp, 55]
	ldrb	w0, [sp, 55]
	cmp	w0, 9
	bls	.L36
	mov	w0, 55
	b	.L37
.L36:
	mov	w0, 48
.L37:
	ldr	w1, [sp, 76]
	sub	w1, w1, #1
	str	w1, [sp, 76]
	ldrb	w1, [sp, 55]
	add	w0, w0, w1
	and	w2, w0, 255
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	strb	w2, [x1, x0]
	ldr	x0, [sp, 88]
	asr	x0, x0, 4
	str	x0, [sp, 88]
	ldr	x0, [sp, 88]
	cmp	x0, 0
	beq	.L38
	ldr	w0, [sp, 76]
	cmp	w0, 0
	bgt	.L39
.L38:
	ldr	w0, [sp, 84]
	cmp	w0, 0
	ble	.L40
	ldr	w0, [sp, 84]
	cmp	w0, 16
	bgt	.L40
	b	.L41
.L42:
	ldr	w0, [sp, 76]
	sub	w0, w0, #1
	str	w0, [sp, 76]
	ldrsw	x0, [sp, 76]
	add	x1, sp, 32
	mov	w2, 48
	strb	w2, [x1, x0]
.L41:
	mov	w1, 16
	ldr	w0, [sp, 84]
	sub	w0, w1, w0
	ldr	w1, [sp, 76]
	cmp	w1, w0
	bgt	.L42
.L40:
	add	x1, sp, 32
	ldrsw	x0, [sp, 76]
	add	x0, x1, x0
	str	x0, [sp, 64]
	b	.L30
.L31:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 115
	bne	.L43
	ldr	w1, [x19, 24]
	ldr	x0, [x19]
	cmp	w1, 0
	blt	.L44
	add	x1, x0, 15
	and	x1, x1, -8
	str	x1, [x19]
	b	.L45
.L44:
	add	w2, w1, 8
	str	w2, [x19, 24]
	ldr	w2, [x19, 24]
	cmp	w2, 0
	ble	.L46
	add	x1, x0, 15
	and	x1, x1, -8
	str	x1, [x19]
	b	.L45
.L46:
	ldr	x2, [x19, 8]
	sxtw	x0, w1
	add	x0, x2, x0
.L45:
	ldr	x0, [x0]
	str	x0, [sp, 64]
.L30:
	ldr	x0, [sp, 64]
	cmp	x0, 0
	bne	.L49
	adrp	x0, .LC0
	add	x0, x0, :lo12:.LC0
	str	x0, [sp, 64]
	b	.L49
.L50:
	ldr	x1, [sp, 64]
	add	x0, x1, 1
	str	x0, [sp, 64]
	ldr	x0, [sp, 24]
	add	x2, x0, 1
	str	x2, [sp, 24]
	ldrb	w1, [x1]
	strb	w1, [x0]
.L49:
	ldr	x0, [sp, 64]
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L50
	b	.L43
.L53:
	nop
	b	.L6
.L54:
	nop
.L6:
	ldr	x0, [sp, 24]
	add	x1, x0, 1
	str	x1, [sp, 24]
	ldr	x1, [sp, 16]
	ldrb	w1, [x1]
	strb	w1, [x0]
.L43:
	ldr	x0, [sp, 16]
	add	x0, x0, 1
	str	x0, [sp, 16]
.L5:
	ldr	x0, [sp, 16]
	ldrb	w0, [x0]
	cmp	w0, 0
	bne	.L51
	ldr	x0, [sp, 24]
	strb	wzr, [x0]
	ldr	x1, [sp, 24]
	ldr	x0, [sp, 56]
	sub	x0, x1, x0
.L52:
	ldr	x19, [sp], 96
	.cfi_restore 19
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	vsprintf, .-vsprintf
	.align	2
	.global	sprintf
	.type	sprintf, %function
sprintf:
.LFB1:
	.cfi_startproc
	stp	x29, x30, [sp, -272]!
	.cfi_def_cfa_offset 272
	.cfi_offset 29, -272
	.cfi_offset 30, -264
	mov	x29, sp
	str	x0, [sp, 56]
	str	x1, [sp, 48]
	str	x2, [sp, 224]
	str	x3, [sp, 232]
	str	x4, [sp, 240]
	str	x5, [sp, 248]
	str	x6, [sp, 256]
	str	x7, [sp, 264]
	str	q0, [sp, 96]
	str	q1, [sp, 112]
	str	q2, [sp, 128]
	str	q3, [sp, 144]
	str	q4, [sp, 160]
	str	q5, [sp, 176]
	str	q6, [sp, 192]
	str	q7, [sp, 208]
	add	x0, sp, 272
	str	x0, [sp, 64]
	add	x0, sp, 272
	str	x0, [sp, 72]
	add	x0, sp, 224
	str	x0, [sp, 80]
	mov	w0, -48
	str	w0, [sp, 88]
	mov	w0, -128
	str	w0, [sp, 92]
	add	x2, sp, 16
	add	x3, sp, 64
	ldp	x0, x1, [x3]
	stp	x0, x1, [x2]
	ldp	x0, x1, [x3, 16]
	stp	x0, x1, [x2, 16]
	add	x0, sp, 16
	mov	x2, x0
	ldr	x1, [sp, 48]
	ldr	x0, [sp, 56]
	bl	vsprintf
	ldp	x29, x30, [sp], 272
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE1:
	.size	sprintf, .-sprintf
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
