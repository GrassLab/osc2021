	.arch armv8-a
	.file	"system_call.c"
	.text
	.align	2
	.global	getpid
	.type	getpid, %function
getpid:
	sub	sp, sp, #16
#APP
// 4 "lib/system_call.c" 1
			svc 1
		mov x0, x0
	
// 0 "" 2
#NO_APP
	str	x0, [sp, 8]
	ldr	x0, [sp, 8]
	add	sp, sp, 16
	ret
	.size	getpid, .-getpid
	.align	2
	.global	uart_read
	.type	uart_read, %function
uart_read:
	sub	sp, sp, #32
	str	x0, [sp, 8]
	str	w1, [sp, 4]
#APP
// 13 "lib/system_call.c" 1
			svc 2
		mov x0, x0
	
// 0 "" 2
#NO_APP
	str	x0, [sp, 24]
	ldr	x0, [sp, 24]
	add	sp, sp, 32
	ret
	.size	uart_read, .-uart_read
	.align	2
	.global	uart_write
	.type	uart_write, %function
uart_write:
	sub	sp, sp, #32
	str	x0, [sp, 8]
	str	w1, [sp, 4]
#APP
// 22 "lib/system_call.c" 1
			svc 3
		mov x0, x0
	
// 0 "" 2
#NO_APP
	str	x0, [sp, 24]
	ldr	x0, [sp, 24]
	add	sp, sp, 32
	ret
	.size	uart_write, .-uart_write
	.align	2
	.global	uart_printf
	.type	uart_printf, %function
uart_printf:
	stp	x29, x30, [sp, -400]!
	add	x29, sp, 0
	str	x0, [x29, 56]
	str	x1, [x29, 344]
	str	x2, [x29, 352]
	str	x3, [x29, 360]
	str	x4, [x29, 368]
	str	x5, [x29, 376]
	str	x6, [x29, 384]
	str	x7, [x29, 392]
	str	q0, [x29, 208]
	str	q1, [x29, 224]
	str	q2, [x29, 240]
	str	q3, [x29, 256]
	str	q4, [x29, 272]
	str	q5, [x29, 288]
	str	q6, [x29, 304]
	str	q7, [x29, 320]
	add	x0, x29, 400
	str	x0, [x29, 72]
	add	x0, x29, 400
	str	x0, [x29, 80]
	add	x0, x29, 336
	str	x0, [x29, 88]
	mov	w0, -56
	str	w0, [x29, 96]
	mov	w0, -128
	str	w0, [x29, 100]
	add	x2, x29, 16
	add	x3, x29, 72
	ldp	x0, x1, [x3]
	stp	x0, x1, [x2]
	ldp	x0, x1, [x3, 16]
	stp	x0, x1, [x2, 16]
	add	x1, x29, 16
	add	x0, x29, 104
	mov	x2, x1
	ldr	x1, [x29, 56]
	bl	vsprintf
	str	w0, [x29, 204]
	ldr	w1, [x29, 204]
	add	x0, x29, 104
	bl	uart_write
	ldr	w0, [x29, 204]
	ldp	x29, x30, [sp], 400
	ret
	.size	uart_printf, .-uart_printf
	.align	2
	.global	exec
	.type	exec, %function
exec:
	sub	sp, sp, #32
	str	x0, [sp, 8]
	str	x1, [sp]
#APP
// 42 "lib/system_call.c" 1
			svc 4
		mov x0, x0
	
// 0 "" 2
#NO_APP
	str	x0, [sp, 24]
	ldr	x0, [sp, 24]
	add	sp, sp, 32
	ret
	.size	exec, .-exec
	.align	2
	.global	exit
	.type	exit, %function
exit:
#APP
// 50 "lib/system_call.c" 1
	svc 5

// 0 "" 2
#NO_APP
	nop
	ret
	.size	exit, .-exit
	.align	2
	.global	fork
	.type	fork, %function
fork:
	sub	sp, sp, #16
#APP
// 55 "lib/system_call.c" 1
			svc 6
		mov x0, x0
	
// 0 "" 2
#NO_APP
	str	x0, [sp, 8]
	ldr	x0, [sp, 8]
	add	sp, sp, 16
	ret
	.size	fork, .-fork
	.align	2
	.global	delay
	.type	delay, %function
delay:
	sub	sp, sp, #16
	str	w0, [sp, 12]
	b	.L15
.L16:
#APP
// 63 "lib/system_call.c" 1
	nop
// 0 "" 2
#NO_APP
.L15:
	ldr	w0, [sp, 12]
	sub	w1, w0, #1
	str	w1, [sp, 12]
	cmp	w0, 0
	bne	.L16
	nop
	add	sp, sp, 16
	ret
	.size	delay, .-delay
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
