dstin		.req	x0
val	    	.req	w1
count		.req	x2
tmp1		.req	x3
tmp1w		.req	w3
tmp2		.req	x4
tmp2w		.req	w4
zva_len_x	.req	x5
zva_len		.req	w5
zva_bits_x	.req	x6

A_l		    .req	x7
A_lw		.req	w7
dst		    .req	x8
tmp3w		.req	w9
tmp3		.req	x9

.global memset
memset:
	mov	dst, dstin	/* Preserve return value.  */
	and	A_lw, val, #255
	orr	A_lw, A_lw, A_lw, lsl #8
	orr	A_lw, A_lw, A_lw, lsl #16
	orr	A_l, A_l, A_l, lsl #32

	cmp	count, #15
	b.hi	.Lover16_proc
	/*All store maybe are non-aligned..*/
	tbz	count, #3, 1f
	str	A_l, [dst], #8
1:
	tbz	count, #2, 2f
	str	A_lw, [dst], #4
2:
	tbz	count, #1, 3f
	strh	A_lw, [dst], #2
3:
	tbz	count, #0, 4f
	strb	A_lw, [dst]
4:
	ret

.Lover16_proc:
	/*Whether  the start address is aligned with 16.*/
	neg	tmp2, dst
	ands	tmp2, tmp2, #15
	b.eq	.Laligned
/*
* The count is not less than 16, we can use stp to store the start 16 bytes,
* then adjust the dst aligned with 16.This process will make the current
* memory address at alignment boundary.
*/
	stp	A_l, A_l, [dst] /*non-aligned store..*/
	/*make the dst aligned..*/
	sub	count, count, tmp2
	add	dst, dst, tmp2

.Laligned:

.Ltail_maybe_long:
	cmp	count, #64
	b.ge	.Lnot_short
.Ltail63:
	ands	tmp1, count, #0x30
	b.eq	3f
	cmp	tmp1w, #0x20
	b.eq	1f
	b.lt	2f
	stp	A_l, A_l, [dst], #16
1:
	stp	A_l, A_l, [dst], #16
2:
	stp	A_l, A_l, [dst], #16
/*
* The last store length is less than 16,use stp to write last 16 bytes.
* It will lead some bytes written twice and the access is non-aligned.
*/
3:
	ands	count, count, #15
	cbz	count, 4f
	add	dst, dst, count
	stp	A_l, A_l, [dst, #-16]	/* Repeat some/all of last store. */
4:
	ret

	/*
	* Critical loop. Start at a new cache line boundary. Assuming
	* 64 bytes per line, this ensures the entire loop is in one line.
	*/
	.p2align	6
.Lnot_short:
	sub	dst, dst, #16/* Pre-bias.  */
	sub	count, count, #64
1:
	stp	A_l, A_l, [dst, #16]
	stp	A_l, A_l, [dst, #32]
	stp	A_l, A_l, [dst, #48]
	stp	A_l, A_l, [dst, #64]!
	subs	count, count, #64
	b.ge	1b
	tst	count, #0x3f
	add	dst, dst, #16
	b.ne	.Ltail63
.Lexitfunc:
	ret
