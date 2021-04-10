
kernel8.elf:     file format elf64-littleaarch64


Disassembly of section .text:

0000000000080000 <el2_to_el1_preserve_sp>:
   80000:	d2b00000 	mov	x0, #0x80000000            	// #2147483648
   80004:	d51c1100 	msr	hcr_el2, x0
   80008:	d28078a0 	mov	x0, #0x3c5                 	// #965
   8000c:	d51c4000 	msr	spsr_el2, x0
   80010:	58001840 	ldr	x0, 80318 <clr_DAIF+0x8>
   80014:	d5181040 	msr	cpacr_el1, x0
   80018:	10000080 	adr	x0, 80028 <el1_set_sp>
   8001c:	d51c4020 	msr	elr_el2, x0
   80020:	910003e0 	mov	x0, sp
   80024:	d69f03e0 	eret

0000000000080028 <el1_set_sp>:
   80028:	9100001f 	mov	sp, x0
   8002c:	d65f03c0 	ret

0000000000080030 <set_el1_evt>:
   80030:	1001fe80 	adr	x0, 84000 <el1_evt>
   80034:	d518c000 	msr	vbar_el1, x0
   80038:	d65f03c0 	ret

000000000008003c <_exc_not_imp>:
   8003c:	a9bf07e0 	stp	x0, x1, [sp, #-16]!
   80040:	a9bf0fe2 	stp	x2, x3, [sp, #-16]!
   80044:	a9bf17e4 	stp	x4, x5, [sp, #-16]!
   80048:	a9bf1fe6 	stp	x6, x7, [sp, #-16]!
   8004c:	a9bf27e8 	stp	x8, x9, [sp, #-16]!
   80050:	a9bf2fea 	stp	x10, x11, [sp, #-16]!
   80054:	a9bf37ec 	stp	x12, x13, [sp, #-16]!
   80058:	a9bf3fee 	stp	x14, x15, [sp, #-16]!
   8005c:	a9bf47f0 	stp	x16, x17, [sp, #-16]!
   80060:	a9bf4ff2 	stp	x18, x19, [sp, #-16]!
   80064:	a9bf57f4 	stp	x20, x21, [sp, #-16]!
   80068:	a9bf5ff6 	stp	x22, x23, [sp, #-16]!
   8006c:	a9bf67f8 	stp	x24, x25, [sp, #-16]!
   80070:	a9bf6ffa 	stp	x26, x27, [sp, #-16]!
   80074:	a9bf77fc 	stp	x28, x29, [sp, #-16]!
   80078:	f81f0ffe 	str	x30, [sp, #-16]!
   8007c:	94000b03 	bl	82c88 <exc_not_imp>
   80080:	f84107fe 	ldr	x30, [sp], #16
   80084:	a8c177fc 	ldp	x28, x29, [sp], #16
   80088:	a8c16ffa 	ldp	x26, x27, [sp], #16
   8008c:	a8c167f8 	ldp	x24, x25, [sp], #16
   80090:	a8c15ff6 	ldp	x22, x23, [sp], #16
   80094:	a8c157f4 	ldp	x20, x21, [sp], #16
   80098:	a8c14ff2 	ldp	x18, x19, [sp], #16
   8009c:	a8c147f0 	ldp	x16, x17, [sp], #16
   800a0:	a8c13fee 	ldp	x14, x15, [sp], #16
   800a4:	a8c137ec 	ldp	x12, x13, [sp], #16
   800a8:	a8c12fea 	ldp	x10, x11, [sp], #16
   800ac:	a8c127e8 	ldp	x8, x9, [sp], #16
   800b0:	a8c11fe6 	ldp	x6, x7, [sp], #16
   800b4:	a8c117e4 	ldp	x4, x5, [sp], #16
   800b8:	a8c10fe2 	ldp	x2, x3, [sp], #16
   800bc:	a8c107e0 	ldp	x0, x1, [sp], #16
   800c0:	d69f03e0 	eret

00000000000800c4 <_syn_handler>:
   800c4:	a9bf07e0 	stp	x0, x1, [sp, #-16]!
   800c8:	a9bf0fe2 	stp	x2, x3, [sp, #-16]!
   800cc:	a9bf17e4 	stp	x4, x5, [sp, #-16]!
   800d0:	a9bf1fe6 	stp	x6, x7, [sp, #-16]!
   800d4:	a9bf27e8 	stp	x8, x9, [sp, #-16]!
   800d8:	a9bf2fea 	stp	x10, x11, [sp, #-16]!
   800dc:	a9bf37ec 	stp	x12, x13, [sp, #-16]!
   800e0:	a9bf3fee 	stp	x14, x15, [sp, #-16]!
   800e4:	a9bf47f0 	stp	x16, x17, [sp, #-16]!
   800e8:	a9bf4ff2 	stp	x18, x19, [sp, #-16]!
   800ec:	a9bf57f4 	stp	x20, x21, [sp, #-16]!
   800f0:	a9bf5ff6 	stp	x22, x23, [sp, #-16]!
   800f4:	a9bf67f8 	stp	x24, x25, [sp, #-16]!
   800f8:	a9bf6ffa 	stp	x26, x27, [sp, #-16]!
   800fc:	a9bf77fc 	stp	x28, x29, [sp, #-16]!
   80100:	f81f0ffe 	str	x30, [sp, #-16]!
   80104:	d5385200 	mrs	x0, esr_el1
   80108:	d5384021 	mrs	x1, elr_el1
   8010c:	d5384102 	mrs	x2, sp_el0
   80110:	d5384003 	mrs	x3, spsr_el1
   80114:	910003e4 	mov	x4, sp
   80118:	94000ae0 	bl	82c98 <syn_handler>
   8011c:	f84107fe 	ldr	x30, [sp], #16
   80120:	a8c177fc 	ldp	x28, x29, [sp], #16
   80124:	a8c16ffa 	ldp	x26, x27, [sp], #16
   80128:	a8c167f8 	ldp	x24, x25, [sp], #16
   8012c:	a8c15ff6 	ldp	x22, x23, [sp], #16
   80130:	a8c157f4 	ldp	x20, x21, [sp], #16
   80134:	a8c14ff2 	ldp	x18, x19, [sp], #16
   80138:	a8c147f0 	ldp	x16, x17, [sp], #16
   8013c:	a8c13fee 	ldp	x14, x15, [sp], #16
   80140:	a8c137ec 	ldp	x12, x13, [sp], #16
   80144:	a8c12fea 	ldp	x10, x11, [sp], #16
   80148:	a8c127e8 	ldp	x8, x9, [sp], #16
   8014c:	a8c11fe6 	ldp	x6, x7, [sp], #16
   80150:	a8c117e4 	ldp	x4, x5, [sp], #16
   80154:	a8c10fe2 	ldp	x2, x3, [sp], #16
   80158:	a8c107e0 	ldp	x0, x1, [sp], #16
   8015c:	d69f03e0 	eret

0000000000080160 <switch_usr>:
   80160:	d5184020 	msr	elr_el1, x0
   80164:	d5184101 	msr	sp_el0, x1
   80168:	d5184002 	msr	spsr_el1, x2
   8016c:	9100007f 	mov	sp, x3
   80170:	f84107fe 	ldr	x30, [sp], #16
   80174:	a8c177fc 	ldp	x28, x29, [sp], #16
   80178:	a8c16ffa 	ldp	x26, x27, [sp], #16
   8017c:	a8c167f8 	ldp	x24, x25, [sp], #16
   80180:	a8c15ff6 	ldp	x22, x23, [sp], #16
   80184:	a8c157f4 	ldp	x20, x21, [sp], #16
   80188:	a8c14ff2 	ldp	x18, x19, [sp], #16
   8018c:	a8c147f0 	ldp	x16, x17, [sp], #16
   80190:	a8c13fee 	ldp	x14, x15, [sp], #16
   80194:	a8c137ec 	ldp	x12, x13, [sp], #16
   80198:	a8c12fea 	ldp	x10, x11, [sp], #16
   8019c:	a8c127e8 	ldp	x8, x9, [sp], #16
   801a0:	a8c11fe6 	ldp	x6, x7, [sp], #16
   801a4:	a8c117e4 	ldp	x4, x5, [sp], #16
   801a8:	a8c10fe2 	ldp	x2, x3, [sp], #16
   801ac:	a8c107e0 	ldp	x0, x1, [sp], #16
   801b0:	d69f03e0 	eret

00000000000801b4 <_irq_handler>:
   801b4:	a9bf07e0 	stp	x0, x1, [sp, #-16]!
   801b8:	a9bf0fe2 	stp	x2, x3, [sp, #-16]!
   801bc:	a9bf17e4 	stp	x4, x5, [sp, #-16]!
   801c0:	a9bf1fe6 	stp	x6, x7, [sp, #-16]!
   801c4:	a9bf27e8 	stp	x8, x9, [sp, #-16]!
   801c8:	a9bf2fea 	stp	x10, x11, [sp, #-16]!
   801cc:	a9bf37ec 	stp	x12, x13, [sp, #-16]!
   801d0:	a9bf3fee 	stp	x14, x15, [sp, #-16]!
   801d4:	a9bf47f0 	stp	x16, x17, [sp, #-16]!
   801d8:	a9bf4ff2 	stp	x18, x19, [sp, #-16]!
   801dc:	a9bf57f4 	stp	x20, x21, [sp, #-16]!
   801e0:	a9bf5ff6 	stp	x22, x23, [sp, #-16]!
   801e4:	a9bf67f8 	stp	x24, x25, [sp, #-16]!
   801e8:	a9bf6ffa 	stp	x26, x27, [sp, #-16]!
   801ec:	a9bf77fc 	stp	x28, x29, [sp, #-16]!
   801f0:	f81f0ffe 	str	x30, [sp, #-16]!
   801f4:	94000ac7 	bl	82d10 <irq_handler>
   801f8:	f84107fe 	ldr	x30, [sp], #16
   801fc:	a8c177fc 	ldp	x28, x29, [sp], #16
   80200:	a8c16ffa 	ldp	x26, x27, [sp], #16
   80204:	a8c167f8 	ldp	x24, x25, [sp], #16
   80208:	a8c15ff6 	ldp	x22, x23, [sp], #16
   8020c:	a8c157f4 	ldp	x20, x21, [sp], #16
   80210:	a8c14ff2 	ldp	x18, x19, [sp], #16
   80214:	a8c147f0 	ldp	x16, x17, [sp], #16
   80218:	a8c13fee 	ldp	x14, x15, [sp], #16
   8021c:	a8c137ec 	ldp	x12, x13, [sp], #16
   80220:	a8c12fea 	ldp	x10, x11, [sp], #16
   80224:	a8c127e8 	ldp	x8, x9, [sp], #16
   80228:	a8c11fe6 	ldp	x6, x7, [sp], #16
   8022c:	a8c117e4 	ldp	x4, x5, [sp], #16
   80230:	a8c10fe2 	ldp	x2, x3, [sp], #16
   80234:	a8c107e0 	ldp	x0, x1, [sp], #16
   80238:	d69f03e0 	eret

000000000008023c <exec_usr>:
   8023c:	d5184020 	msr	elr_el1, x0
   80240:	d5184101 	msr	sp_el0, x1
   80244:	d5184002 	msr	spsr_el1, x2
   80248:	a9bf53f3 	stp	x19, x20, [sp, #-16]!
   8024c:	a9bf5bf5 	stp	x21, x22, [sp, #-16]!
   80250:	a9bf63f7 	stp	x23, x24, [sp, #-16]!
   80254:	a9bf6bf9 	stp	x25, x26, [sp, #-16]!
   80258:	a9bf73fb 	stp	x27, x28, [sp, #-16]!
   8025c:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   80260:	1000041e 	adr	x30, 802e0 <_force_exit>
   80264:	d2800000 	mov	x0, #0x0                   	// #0
   80268:	d2800001 	mov	x1, #0x0                   	// #0
   8026c:	d2800002 	mov	x2, #0x0                   	// #0
   80270:	d2800003 	mov	x3, #0x0                   	// #0
   80274:	d2800004 	mov	x4, #0x0                   	// #0
   80278:	d2800005 	mov	x5, #0x0                   	// #0
   8027c:	d2800006 	mov	x6, #0x0                   	// #0
   80280:	d2800007 	mov	x7, #0x0                   	// #0
   80284:	d2800008 	mov	x8, #0x0                   	// #0
   80288:	d2800009 	mov	x9, #0x0                   	// #0
   8028c:	d280000a 	mov	x10, #0x0                   	// #0
   80290:	d280000b 	mov	x11, #0x0                   	// #0
   80294:	d280000c 	mov	x12, #0x0                   	// #0
   80298:	d280000d 	mov	x13, #0x0                   	// #0
   8029c:	d280000e 	mov	x14, #0x0                   	// #0
   802a0:	d280000f 	mov	x15, #0x0                   	// #0
   802a4:	d2800010 	mov	x16, #0x0                   	// #0
   802a8:	d2800011 	mov	x17, #0x0                   	// #0
   802ac:	d2800012 	mov	x18, #0x0                   	// #0
   802b0:	d2800013 	mov	x19, #0x0                   	// #0
   802b4:	d2800014 	mov	x20, #0x0                   	// #0
   802b8:	d2800015 	mov	x21, #0x0                   	// #0
   802bc:	d2800016 	mov	x22, #0x0                   	// #0
   802c0:	d2800017 	mov	x23, #0x0                   	// #0
   802c4:	d2800018 	mov	x24, #0x0                   	// #0
   802c8:	d2800019 	mov	x25, #0x0                   	// #0
   802cc:	d280001a 	mov	x26, #0x0                   	// #0
   802d0:	d280001b 	mov	x27, #0x0                   	// #0
   802d4:	d280001c 	mov	x28, #0x0                   	// #0
   802d8:	d280001d 	mov	x29, #0x0                   	// #0
   802dc:	d69f03e0 	eret

00000000000802e0 <_force_exit>:
   802e0:	d4000781 	svc	#0x3c

00000000000802e4 <ret_kern>:
   802e4:	9100001f 	mov	sp, x0
   802e8:	910403ff 	add	sp, sp, #0x100
   802ec:	a8c17bfd 	ldp	x29, x30, [sp], #16
   802f0:	a8c173fb 	ldp	x27, x28, [sp], #16
   802f4:	a8c16bf9 	ldp	x25, x26, [sp], #16
   802f8:	a8c163f7 	ldp	x23, x24, [sp], #16
   802fc:	a8c15bf5 	ldp	x21, x22, [sp], #16
   80300:	a8c153f3 	ldp	x19, x20, [sp], #16
   80304:	d65f03c0 	ret

0000000000080308 <set_DAIF>:
   80308:	d5034fdf 	msr	daifset, #0xf
   8030c:	d65f03c0 	ret

0000000000080310 <clr_DAIF>:
   80310:	d5034fff 	msr	daifclr, #0xf
   80314:	d65f03c0 	ret
   80318:	00330000 	.word	0x00330000
   8031c:	00000000 	.word	0x00000000

0000000000080320 <place_buddy.constprop.1>:
   80320:	b0000024 	adrp	x4, 85000 <el1_evt+0x1000>
   80324:	dac0000e 	rbit	x14, x0
   80328:	dac0002d 	rbit	x13, x1
   8032c:	dac011ce 	clz	x14, x14
   80330:	f9417484 	ldr	x4, [x4, #744]
   80334:	eb01001f 	cmp	x0, x1
   80338:	dac011ad 	clz	x13, x13
   8033c:	528001ec 	mov	w12, #0xf                   	// #15
   80340:	5280002b 	mov	w11, #0x1                   	// #1
   80344:	540004a0 	b.eq	803d8 <place_buddy.constprop.1+0xb8>  // b.none
   80348:	510031c3 	sub	w3, w14, #0xc
   8034c:	510031a2 	sub	w2, w13, #0xc
   80350:	71003c7f 	cmp	w3, #0xf
   80354:	d34cfc0a 	lsr	x10, x0, #12
   80358:	1a8cd063 	csel	w3, w3, w12, le
   8035c:	71003c5f 	cmp	w2, #0xf
   80360:	1a8cd042 	csel	w2, w2, w12, le
   80364:	11003067 	add	w7, w3, #0xc
   80368:	11003046 	add	w6, w2, #0xc
   8036c:	937c7c68 	sbfiz	x8, x3, #4, #32
   80370:	937c7c45 	sbfiz	x5, x2, #4, #32
   80374:	8b040110 	add	x16, x8, x4
   80378:	8b0400af 	add	x15, x5, x4
   8037c:	6b02007f 	cmp	w3, w2
   80380:	1ac62166 	lsl	w6, w11, w6
   80384:	1ac72167 	lsl	w7, w11, w7
   80388:	f9408089 	ldr	x9, [x4, #256]
   8038c:	5400028c 	b.gt	803dc <place_buddy.constprop.1+0xbc>
   80390:	386a6922 	ldrb	w2, [x9, x10]
   80394:	12001042 	and	w2, w2, #0x1f
   80398:	382a6922 	strb	w2, [x9, x10]
   8039c:	f9408085 	ldr	x5, [x4, #256]
   803a0:	386a68a2 	ldrb	w2, [x5, x10]
   803a4:	121b6842 	and	w2, w2, #0xffffffe0
   803a8:	2a030043 	orr	w3, w2, w3
   803ac:	382a68a3 	strb	w3, [x5, x10]
   803b0:	f8646902 	ldr	x2, [x8, x4]
   803b4:	a9004002 	stp	x2, x16, [x0]
   803b8:	f8646902 	ldr	x2, [x8, x4]
   803bc:	f9000440 	str	x0, [x2, #8]
   803c0:	f8246900 	str	x0, [x8, x4]
   803c4:	8b27c000 	add	x0, x0, w7, sxtw
   803c8:	dac0000e 	rbit	x14, x0
   803cc:	eb01001f 	cmp	x0, x1
   803d0:	dac011ce 	clz	x14, x14
   803d4:	54fffba1 	b.ne	80348 <place_buddy.constprop.1+0x28>  // b.any
   803d8:	d65f03c0 	ret
   803dc:	cb26c021 	sub	x1, x1, w6, sxtw
   803e0:	dac0002d 	rbit	x13, x1
   803e4:	eb00003f 	cmp	x1, x0
   803e8:	d34cfc26 	lsr	x6, x1, #12
   803ec:	dac011ad 	clz	x13, x13
   803f0:	38666923 	ldrb	w3, [x9, x6]
   803f4:	12001063 	and	w3, w3, #0x1f
   803f8:	38266923 	strb	w3, [x9, x6]
   803fc:	f9408087 	ldr	x7, [x4, #256]
   80400:	386668e3 	ldrb	w3, [x7, x6]
   80404:	121b6863 	and	w3, w3, #0xffffffe0
   80408:	2a020062 	orr	w2, w3, w2
   8040c:	382668e2 	strb	w2, [x7, x6]
   80410:	f86468a2 	ldr	x2, [x5, x4]
   80414:	a9003c22 	stp	x2, x15, [x1]
   80418:	f86468a2 	ldr	x2, [x5, x4]
   8041c:	f9000441 	str	x1, [x2, #8]
   80420:	f82468a1 	str	x1, [x5, x4]
   80424:	54fff921 	b.ne	80348 <place_buddy.constprop.1+0x28>  // b.any
   80428:	d65f03c0 	ret
   8042c:	d503201f 	nop

0000000000080430 <sort_reserve>:
   80430:	d0000020 	adrp	x0, 86000 <sa>
   80434:	9100000c 	add	x12, x0, #0x0
   80438:	f9408181 	ldr	x1, [x12, #256]
   8043c:	71000421 	subs	w1, w1, #0x1
   80440:	540044c4 	b.mi	80cd8 <sort_reserve+0x8a8>  // b.first
   80444:	a9ad7bfd 	stp	x29, x30, [sp, #-304]!
   80448:	52800012 	mov	w18, #0x0                   	// #0
   8044c:	5280000f 	mov	w15, #0x0                   	// #0
   80450:	910423e2 	add	x2, sp, #0x108
   80454:	910473e3 	add	x3, sp, #0x11c
   80458:	910003fd 	mov	x29, sp
   8045c:	a90153f3 	stp	x19, x20, [sp, #16]
   80460:	5280000a 	mov	w10, #0x0                   	// #0
   80464:	52800014 	mov	w20, #0x0                   	// #0
   80468:	a9025bf5 	stp	x21, x22, [sp, #32]
   8046c:	52800008 	mov	w8, #0x0                   	// #0
   80470:	52800006 	mov	w6, #0x0                   	// #0
   80474:	a90363f7 	stp	x23, x24, [sp, #48]
   80478:	52800017 	mov	w23, #0x0                   	// #0
   8047c:	a9046bf9 	stp	x25, x26, [sp, #64]
   80480:	5280001a 	mov	w26, #0x0                   	// #0
   80484:	a90573fb 	stp	x27, x28, [sp, #80]
   80488:	293f7c5f 	stp	wzr, wzr, [x2, #-8]
   8048c:	293f7c7f 	stp	wzr, wzr, [x3, #-8]
   80490:	2900fc5f 	stp	wzr, wzr, [x2, #4]
   80494:	2900fc7f 	stp	wzr, wzr, [x3, #4]
   80498:	b90077ff 	str	wzr, [sp, #116]
   8049c:	29137fff 	stp	wzr, wzr, [sp, #152]
   804a0:	29197fff 	stp	wzr, wzr, [sp, #200]
   804a4:	b900d7ff 	str	wzr, [sp, #212]
   804a8:	291d7fff 	stp	wzr, wzr, [sp, #232]
   804ac:	291f7fff 	stp	wzr, wzr, [sp, #248]
   804b0:	f9400000 	ldr	x0, [x0]
   804b4:	b90073ff 	str	wzr, [sp, #112]
   804b8:	f9402d82 	ldr	x2, [x12, #88]
   804bc:	f90043e2 	str	x2, [sp, #128]
   804c0:	f9403182 	ldr	x2, [x12, #96]
   804c4:	f9004be2 	str	x2, [sp, #144]
   804c8:	f9403582 	ldr	x2, [x12, #104]
   804cc:	f9005be2 	str	x2, [sp, #176]
   804d0:	f9403982 	ldr	x2, [x12, #112]
   804d4:	f90057e2 	str	x2, [sp, #168]
   804d8:	f9403d82 	ldr	x2, [x12, #120]
   804dc:	f90063e2 	str	x2, [sp, #192]
   804e0:	b900d3ff 	str	wzr, [sp, #208]
   804e4:	b9010bff 	str	wzr, [sp, #264]
   804e8:	b9011fff 	str	wzr, [sp, #284]
   804ec:	b9012bff 	str	wzr, [sp, #296]
   804f0:	b9012fff 	str	wzr, [sp, #300]
   804f4:	a9409583 	ldp	x3, x5, [x12, #8]
   804f8:	a941a587 	ldp	x7, x9, [x12, #24]
   804fc:	a942c18b 	ldp	x11, x16, [x12, #40]
   80500:	a943d59e 	ldp	x30, x21, [x12, #56]
   80504:	a944ed98 	ldp	x24, x27, [x12, #72]
   80508:	f9404182 	ldr	x2, [x12, #128]
   8050c:	a948b984 	ldp	x4, x14, [x12, #136]
   80510:	a949cd91 	ldp	x17, x19, [x12, #152]
   80514:	a94ae596 	ldp	x22, x25, [x12, #168]
   80518:	a94bb59c 	ldp	x28, x13, [x12, #184]
   8051c:	a90637fb 	stp	x27, x13, [sp, #96]
   80520:	f940658d 	ldr	x13, [x12, #200]
   80524:	f9003fed 	str	x13, [sp, #120]
   80528:	f940698d 	ldr	x13, [x12, #208]
   8052c:	f90047ed 	str	x13, [sp, #136]
   80530:	f9406d8d 	ldr	x13, [x12, #216]
   80534:	f90053ed 	str	x13, [sp, #160]
   80538:	f940718d 	ldr	x13, [x12, #224]
   8053c:	f9005fed 	str	x13, [sp, #184]
   80540:	f940758d 	ldr	x13, [x12, #232]
   80544:	f9006fed 	str	x13, [sp, #216]
   80548:	a94f318d 	ldp	x13, x12, [x12, #240]
   8054c:	f90073ed 	str	x13, [sp, #224]
   80550:	f9007bec 	str	x12, [sp, #240]
   80554:	14000084 	b	80764 <sort_reserve+0x334>
   80558:	aa0403ed 	mov	x13, x4
   8055c:	aa0303ec 	mov	x12, x3
   80560:	aa0203e4 	mov	x4, x2
   80564:	aa0003e3 	mov	x3, x0
   80568:	aa0d03e2 	mov	x2, x13
   8056c:	aa0c03e0 	mov	x0, x12
   80570:	7100043f 	cmp	w1, #0x1
   80574:	540010a0 	b.eq	80788 <sort_reserve+0x358>  // b.none
   80578:	eb05001f 	cmp	x0, x5
   8057c:	540010c8 	b.hi	80794 <sort_reserve+0x364>  // b.pmore
   80580:	aa0203ed 	mov	x13, x2
   80584:	aa0003ec 	mov	x12, x0
   80588:	aa0e03e2 	mov	x2, x14
   8058c:	aa0503e0 	mov	x0, x5
   80590:	7100083f 	cmp	w1, #0x2
   80594:	540010e0 	b.eq	807b0 <sort_reserve+0x380>  // b.none
   80598:	eb07001f 	cmp	x0, x7
   8059c:	54001108 	b.hi	807bc <sort_reserve+0x38c>  // b.pmore
   805a0:	aa0203ee 	mov	x14, x2
   805a4:	aa0003e5 	mov	x5, x0
   805a8:	aa1103e2 	mov	x2, x17
   805ac:	aa0703e0 	mov	x0, x7
   805b0:	71000c3f 	cmp	w1, #0x3
   805b4:	54001140 	b.eq	807dc <sort_reserve+0x3ac>  // b.none
   805b8:	eb09001f 	cmp	x0, x9
   805bc:	54001168 	b.hi	807e8 <sort_reserve+0x3b8>  // b.pmore
   805c0:	aa0203f1 	mov	x17, x2
   805c4:	aa0003e7 	mov	x7, x0
   805c8:	aa1303e2 	mov	x2, x19
   805cc:	aa0903e0 	mov	x0, x9
   805d0:	7100103f 	cmp	w1, #0x4
   805d4:	54001180 	b.eq	80804 <sort_reserve+0x3d4>  // b.none
   805d8:	eb0b001f 	cmp	x0, x11
   805dc:	540011a8 	b.hi	80810 <sort_reserve+0x3e0>  // b.pmore
   805e0:	aa0203f3 	mov	x19, x2
   805e4:	aa0003e9 	mov	x9, x0
   805e8:	aa1603e2 	mov	x2, x22
   805ec:	aa0b03e0 	mov	x0, x11
   805f0:	7100143f 	cmp	w1, #0x5
   805f4:	540011e0 	b.eq	80830 <sort_reserve+0x400>  // b.none
   805f8:	eb10001f 	cmp	x0, x16
   805fc:	54001208 	b.hi	8083c <sort_reserve+0x40c>  // b.pmore
   80600:	aa0203f6 	mov	x22, x2
   80604:	aa0003eb 	mov	x11, x0
   80608:	aa1903e2 	mov	x2, x25
   8060c:	aa1003e0 	mov	x0, x16
   80610:	7100183f 	cmp	w1, #0x6
   80614:	54001220 	b.eq	80858 <sort_reserve+0x428>  // b.none
   80618:	eb1e001f 	cmp	x0, x30
   8061c:	54001248 	b.hi	80864 <sort_reserve+0x434>  // b.pmore
   80620:	aa0203f9 	mov	x25, x2
   80624:	aa0003f0 	mov	x16, x0
   80628:	aa1c03e2 	mov	x2, x28
   8062c:	aa1e03e0 	mov	x0, x30
   80630:	71001c3f 	cmp	w1, #0x7
   80634:	54001280 	b.eq	80884 <sort_reserve+0x454>  // b.none
   80638:	eb15001f 	cmp	x0, x21
   8063c:	540012a8 	b.hi	80890 <sort_reserve+0x460>  // b.pmore
   80640:	aa0203fc 	mov	x28, x2
   80644:	aa0003fe 	mov	x30, x0
   80648:	7100203f 	cmp	w1, #0x8
   8064c:	aa1503e0 	mov	x0, x21
   80650:	f94037e2 	ldr	x2, [sp, #104]
   80654:	540012c0 	b.eq	808ac <sort_reserve+0x47c>  // b.none
   80658:	eb18001f 	cmp	x0, x24
   8065c:	540012e8 	b.hi	808b8 <sort_reserve+0x488>  // b.pmore
   80660:	f90037e2 	str	x2, [sp, #104]
   80664:	aa0003f5 	mov	x21, x0
   80668:	7100243f 	cmp	w1, #0x9
   8066c:	aa1803e0 	mov	x0, x24
   80670:	f9403fe2 	ldr	x2, [sp, #120]
   80674:	54001340 	b.eq	808dc <sort_reserve+0x4ac>  // b.none
   80678:	f94033fb 	ldr	x27, [sp, #96]
   8067c:	eb1b001f 	cmp	x0, x27
   80680:	54001348 	b.hi	808e8 <sort_reserve+0x4b8>  // b.pmore
   80684:	f9003fe2 	str	x2, [sp, #120]
   80688:	aa0003f8 	mov	x24, x0
   8068c:	7100283f 	cmp	w1, #0xa
   80690:	aa1b03e0 	mov	x0, x27
   80694:	f94047e2 	ldr	x2, [sp, #136]
   80698:	540013a0 	b.eq	8090c <sort_reserve+0x4dc>  // b.none
   8069c:	f94043fb 	ldr	x27, [sp, #128]
   806a0:	eb1b001f 	cmp	x0, x27
   806a4:	540013a8 	b.hi	80918 <sort_reserve+0x4e8>  // b.pmore
   806a8:	f90033e0 	str	x0, [sp, #96]
   806ac:	71002c3f 	cmp	w1, #0xb
   806b0:	f90047e2 	str	x2, [sp, #136]
   806b4:	aa1b03e0 	mov	x0, x27
   806b8:	f94053e2 	ldr	x2, [sp, #160]
   806bc:	54001440 	b.eq	80944 <sort_reserve+0x514>  // b.none
   806c0:	f9404bfb 	ldr	x27, [sp, #144]
   806c4:	eb1b001f 	cmp	x0, x27
   806c8:	54001448 	b.hi	80950 <sort_reserve+0x520>  // b.pmore
   806cc:	f90043e0 	str	x0, [sp, #128]
   806d0:	7100303f 	cmp	w1, #0xc
   806d4:	f90053e2 	str	x2, [sp, #160]
   806d8:	aa1b03e0 	mov	x0, x27
   806dc:	f9405fe2 	ldr	x2, [sp, #184]
   806e0:	540014e0 	b.eq	8097c <sort_reserve+0x54c>  // b.none
   806e4:	f9405bfb 	ldr	x27, [sp, #176]
   806e8:	eb1b001f 	cmp	x0, x27
   806ec:	540014e8 	b.hi	80988 <sort_reserve+0x558>  // b.pmore
   806f0:	f9004be0 	str	x0, [sp, #144]
   806f4:	7100343f 	cmp	w1, #0xd
   806f8:	f9005fe2 	str	x2, [sp, #184]
   806fc:	aa1b03e0 	mov	x0, x27
   80700:	f9406fe2 	ldr	x2, [sp, #216]
   80704:	54001580 	b.eq	809b4 <sort_reserve+0x584>  // b.none
   80708:	f94057fb 	ldr	x27, [sp, #168]
   8070c:	eb1b001f 	cmp	x0, x27
   80710:	54002b48 	b.hi	80c78 <sort_reserve+0x848>  // b.pmore
   80714:	f9005be0 	str	x0, [sp, #176]
   80718:	7100383f 	cmp	w1, #0xe
   8071c:	f9006fe2 	str	x2, [sp, #216]
   80720:	aa1b03e0 	mov	x0, x27
   80724:	f94073e2 	ldr	x2, [sp, #224]
   80728:	54002be0 	b.eq	80ca4 <sort_reserve+0x874>  // b.none
   8072c:	f94063fb 	ldr	x27, [sp, #192]
   80730:	eb1b001f 	cmp	x0, x27
   80734:	54002be8 	b.hi	80cb0 <sort_reserve+0x880>  // b.pmore
   80738:	f90073e2 	str	x2, [sp, #224]
   8073c:	f9407be2 	ldr	x2, [sp, #240]
   80740:	f90057e0 	str	x0, [sp, #168]
   80744:	aa1b03e0 	mov	x0, x27
   80748:	f90063e0 	str	x0, [sp, #192]
   8074c:	f9007be2 	str	x2, [sp, #240]
   80750:	aa0403e2 	mov	x2, x4
   80754:	aa0303e0 	mov	x0, x3
   80758:	aa0d03e4 	mov	x4, x13
   8075c:	aa0c03e3 	mov	x3, x12
   80760:	51000421 	sub	w1, w1, #0x1
   80764:	7100003f 	cmp	w1, #0x0
   80768:	540012cd 	b.le	809c0 <sort_reserve+0x590>
   8076c:	eb00007f 	cmp	x3, x0
   80770:	54ffef42 	b.cs	80558 <sort_reserve+0x128>  // b.hs, b.nlast
   80774:	52800026 	mov	w6, #0x1                   	// #1
   80778:	b90073e6 	str	w6, [sp, #112]
   8077c:	291a1be6 	stp	w6, w6, [sp, #208]
   80780:	7100043f 	cmp	w1, #0x1
   80784:	54ffefa1 	b.ne	80578 <sort_reserve+0x148>  // b.any
   80788:	aa0203ed 	mov	x13, x2
   8078c:	aa0003ec 	mov	x12, x0
   80790:	17fffff0 	b	80750 <sort_reserve+0x320>
   80794:	52800028 	mov	w8, #0x1                   	// #1
   80798:	290e23e8 	stp	w8, w8, [sp, #112]
   8079c:	aa0e03ed 	mov	x13, x14
   807a0:	aa0503ec 	mov	x12, x5
   807a4:	2a0803e6 	mov	w6, w8
   807a8:	7100083f 	cmp	w1, #0x2
   807ac:	54ffef61 	b.ne	80598 <sort_reserve+0x168>  // b.any
   807b0:	aa0203ee 	mov	x14, x2
   807b4:	aa0003e5 	mov	x5, x0
   807b8:	17ffffe6 	b	80750 <sort_reserve+0x320>
   807bc:	5280002a 	mov	w10, #0x1                   	// #1
   807c0:	b90077ea 	str	w10, [sp, #116]
   807c4:	b9009bea 	str	w10, [sp, #152]
   807c8:	aa1103ee 	mov	x14, x17
   807cc:	aa0703e5 	mov	x5, x7
   807d0:	2a0a03e8 	mov	w8, w10
   807d4:	71000c3f 	cmp	w1, #0x3
   807d8:	54ffef01 	b.ne	805b8 <sort_reserve+0x188>  // b.any
   807dc:	aa0203f1 	mov	x17, x2
   807e0:	aa0003e7 	mov	x7, x0
   807e4:	17ffffdb 	b	80750 <sort_reserve+0x320>
   807e8:	5280002f 	mov	w15, #0x1                   	// #1
   807ec:	29133fef 	stp	w15, w15, [sp, #152]
   807f0:	aa1303f1 	mov	x17, x19
   807f4:	aa0903e7 	mov	x7, x9
   807f8:	2a0f03ea 	mov	w10, w15
   807fc:	7100103f 	cmp	w1, #0x4
   80800:	54ffeec1 	b.ne	805d8 <sort_reserve+0x1a8>  // b.any
   80804:	aa0203f3 	mov	x19, x2
   80808:	aa0003e9 	mov	x9, x0
   8080c:	17ffffd1 	b	80750 <sort_reserve+0x320>
   80810:	52800032 	mov	w18, #0x1                   	// #1
   80814:	b9009ff2 	str	w18, [sp, #156]
   80818:	b900cbf2 	str	w18, [sp, #200]
   8081c:	aa1603f3 	mov	x19, x22
   80820:	aa0b03e9 	mov	x9, x11
   80824:	2a1203ef 	mov	w15, w18
   80828:	7100143f 	cmp	w1, #0x5
   8082c:	54ffee61 	b.ne	805f8 <sort_reserve+0x1c8>  // b.any
   80830:	aa0203f6 	mov	x22, x2
   80834:	aa0003eb 	mov	x11, x0
   80838:	17ffffc6 	b	80750 <sort_reserve+0x320>
   8083c:	52800034 	mov	w20, #0x1                   	// #1
   80840:	291953f4 	stp	w20, w20, [sp, #200]
   80844:	aa1903f6 	mov	x22, x25
   80848:	aa1003eb 	mov	x11, x16
   8084c:	2a1403f2 	mov	w18, w20
   80850:	7100183f 	cmp	w1, #0x6
   80854:	54ffee21 	b.ne	80618 <sort_reserve+0x1e8>  // b.any
   80858:	aa0203f9 	mov	x25, x2
   8085c:	aa0003f0 	mov	x16, x0
   80860:	17ffffbc 	b	80750 <sort_reserve+0x320>
   80864:	52800037 	mov	w23, #0x1                   	// #1
   80868:	b900cff7 	str	w23, [sp, #204]
   8086c:	b900ebf7 	str	w23, [sp, #232]
   80870:	aa1c03f9 	mov	x25, x28
   80874:	aa1e03f0 	mov	x16, x30
   80878:	2a1703f4 	mov	w20, w23
   8087c:	71001c3f 	cmp	w1, #0x7
   80880:	54ffedc1 	b.ne	80638 <sort_reserve+0x208>  // b.any
   80884:	aa0203fc 	mov	x28, x2
   80888:	aa0003fe 	mov	x30, x0
   8088c:	17ffffb1 	b	80750 <sort_reserve+0x320>
   80890:	5280003a 	mov	w26, #0x1                   	// #1
   80894:	291d6bfa 	stp	w26, w26, [sp, #232]
   80898:	aa1503fe 	mov	x30, x21
   8089c:	2a1a03f7 	mov	w23, w26
   808a0:	7100203f 	cmp	w1, #0x8
   808a4:	f94037fc 	ldr	x28, [sp, #104]
   808a8:	54ffed81 	b.ne	80658 <sort_reserve+0x228>  // b.any
   808ac:	aa0003f5 	mov	x21, x0
   808b0:	f90037e2 	str	x2, [sp, #104]
   808b4:	17ffffa7 	b	80750 <sort_reserve+0x320>
   808b8:	f9403ff5 	ldr	x21, [sp, #120]
   808bc:	f90037f5 	str	x21, [sp, #104]
   808c0:	aa1803f5 	mov	x21, x24
   808c4:	52800038 	mov	w24, #0x1                   	// #1
   808c8:	b900eff8 	str	w24, [sp, #236]
   808cc:	2a1803fa 	mov	w26, w24
   808d0:	291f63f8 	stp	w24, w24, [sp, #248]
   808d4:	7100243f 	cmp	w1, #0x9
   808d8:	54ffed01 	b.ne	80678 <sort_reserve+0x248>  // b.any
   808dc:	aa0003f8 	mov	x24, x0
   808e0:	f9003fe2 	str	x2, [sp, #120]
   808e4:	17ffff9b 	b	80750 <sort_reserve+0x320>
   808e8:	f94047f8 	ldr	x24, [sp, #136]
   808ec:	5280003b 	mov	w27, #0x1                   	// #1
   808f0:	f9003ff8 	str	x24, [sp, #120]
   808f4:	7100283f 	cmp	w1, #0xa
   808f8:	291f6ffb 	stp	w27, w27, [sp, #248]
   808fc:	b90103fb 	str	w27, [sp, #256]
   80900:	b90107fb 	str	w27, [sp, #260]
   80904:	f94033f8 	ldr	x24, [sp, #96]
   80908:	54ffeca1 	b.ne	8069c <sort_reserve+0x26c>  // b.any
   8090c:	f90033e0 	str	x0, [sp, #96]
   80910:	f90047e2 	str	x2, [sp, #136]
   80914:	17ffff8f 	b	80750 <sort_reserve+0x320>
   80918:	f94053fb 	ldr	x27, [sp, #160]
   8091c:	f90047fb 	str	x27, [sp, #136]
   80920:	f94043fb 	ldr	x27, [sp, #128]
   80924:	f90033fb 	str	x27, [sp, #96]
   80928:	5280003b 	mov	w27, #0x1                   	// #1
   8092c:	b90103fb 	str	w27, [sp, #256]
   80930:	b90107fb 	str	w27, [sp, #260]
   80934:	71002c3f 	cmp	w1, #0xb
   80938:	b9010ffb 	str	w27, [sp, #268]
   8093c:	b90113fb 	str	w27, [sp, #272]
   80940:	54ffec01 	b.ne	806c0 <sort_reserve+0x290>  // b.any
   80944:	f90043e0 	str	x0, [sp, #128]
   80948:	f90053e2 	str	x2, [sp, #160]
   8094c:	17ffff81 	b	80750 <sort_reserve+0x320>
   80950:	f9405ffb 	ldr	x27, [sp, #184]
   80954:	f90053fb 	str	x27, [sp, #160]
   80958:	f9404bfb 	ldr	x27, [sp, #144]
   8095c:	f90043fb 	str	x27, [sp, #128]
   80960:	5280003b 	mov	w27, #0x1                   	// #1
   80964:	b9010ffb 	str	w27, [sp, #268]
   80968:	b90113fb 	str	w27, [sp, #272]
   8096c:	7100303f 	cmp	w1, #0xc
   80970:	b90117fb 	str	w27, [sp, #276]
   80974:	b9011bfb 	str	w27, [sp, #280]
   80978:	54ffeb61 	b.ne	806e4 <sort_reserve+0x2b4>  // b.any
   8097c:	f9004be0 	str	x0, [sp, #144]
   80980:	f9005fe2 	str	x2, [sp, #184]
   80984:	17ffff73 	b	80750 <sort_reserve+0x320>
   80988:	f9406ffb 	ldr	x27, [sp, #216]
   8098c:	f9005ffb 	str	x27, [sp, #184]
   80990:	f9405bfb 	ldr	x27, [sp, #176]
   80994:	f9004bfb 	str	x27, [sp, #144]
   80998:	5280003b 	mov	w27, #0x1                   	// #1
   8099c:	b90117fb 	str	w27, [sp, #276]
   809a0:	b9011bfb 	str	w27, [sp, #280]
   809a4:	7100343f 	cmp	w1, #0xd
   809a8:	b90123fb 	str	w27, [sp, #288]
   809ac:	b90127fb 	str	w27, [sp, #292]
   809b0:	54ffeac1 	b.ne	80708 <sort_reserve+0x2d8>  // b.any
   809b4:	f9005be0 	str	x0, [sp, #176]
   809b8:	f9006fe2 	str	x2, [sp, #216]
   809bc:	17ffff65 	b	80750 <sort_reserve+0x320>
   809c0:	71000421 	subs	w1, w1, #0x1
   809c4:	54ffed05 	b.pl	80764 <sort_reserve+0x334>  // b.nfrst
   809c8:	b9410be1 	ldr	w1, [sp, #264]
   809cc:	f94033fb 	ldr	x27, [sp, #96]
   809d0:	340000a1 	cbz	w1, 809e4 <sort_reserve+0x5b4>
   809d4:	d0000021 	adrp	x1, 86000 <sa>
   809d8:	91000021 	add	x1, x1, #0x0
   809dc:	f94057ec 	ldr	x12, [sp, #168]
   809e0:	f900382c 	str	x12, [x1, #112]
   809e4:	b9412be1 	ldr	w1, [sp, #296]
   809e8:	340000a1 	cbz	w1, 809fc <sort_reserve+0x5cc>
   809ec:	d0000021 	adrp	x1, 86000 <sa>
   809f0:	91000021 	add	x1, x1, #0x0
   809f4:	f94063ec 	ldr	x12, [sp, #192]
   809f8:	f9003c2c 	str	x12, [x1, #120]
   809fc:	b9411fe1 	ldr	w1, [sp, #284]
   80a00:	340000a1 	cbz	w1, 80a14 <sort_reserve+0x5e4>
   80a04:	d0000021 	adrp	x1, 86000 <sa>
   80a08:	91000021 	add	x1, x1, #0x0
   80a0c:	f94073ec 	ldr	x12, [sp, #224]
   80a10:	f900782c 	str	x12, [x1, #240]
   80a14:	b9412fe1 	ldr	w1, [sp, #300]
   80a18:	340000a1 	cbz	w1, 80a2c <sort_reserve+0x5fc>
   80a1c:	d0000021 	adrp	x1, 86000 <sa>
   80a20:	91000021 	add	x1, x1, #0x0
   80a24:	f9407bec 	ldr	x12, [sp, #240]
   80a28:	f9007c2c 	str	x12, [x1, #248]
   80a2c:	b940d3e1 	ldr	w1, [sp, #208]
   80a30:	34000061 	cbz	w1, 80a3c <sort_reserve+0x60c>
   80a34:	d0000021 	adrp	x1, 86000 <sa>
   80a38:	f9000020 	str	x0, [x1]
   80a3c:	b94073e0 	ldr	w0, [sp, #112]
   80a40:	34000080 	cbz	w0, 80a50 <sort_reserve+0x620>
   80a44:	d0000020 	adrp	x0, 86000 <sa>
   80a48:	91000000 	add	x0, x0, #0x0
   80a4c:	f9000403 	str	x3, [x0, #8]
   80a50:	b940d7e0 	ldr	w0, [sp, #212]
   80a54:	34000080 	cbz	w0, 80a64 <sort_reserve+0x634>
   80a58:	d0000020 	adrp	x0, 86000 <sa>
   80a5c:	91000000 	add	x0, x0, #0x0
   80a60:	f9004002 	str	x2, [x0, #128]
   80a64:	34000086 	cbz	w6, 80a74 <sort_reserve+0x644>
   80a68:	d0000020 	adrp	x0, 86000 <sa>
   80a6c:	91000000 	add	x0, x0, #0x0
   80a70:	f9004404 	str	x4, [x0, #136]
   80a74:	b94077e0 	ldr	w0, [sp, #116]
   80a78:	34000080 	cbz	w0, 80a88 <sort_reserve+0x658>
   80a7c:	d0000020 	adrp	x0, 86000 <sa>
   80a80:	91000000 	add	x0, x0, #0x0
   80a84:	f9000805 	str	x5, [x0, #16]
   80a88:	34000088 	cbz	w8, 80a98 <sort_reserve+0x668>
   80a8c:	d0000020 	adrp	x0, 86000 <sa>
   80a90:	91000000 	add	x0, x0, #0x0
   80a94:	f900480e 	str	x14, [x0, #144]
   80a98:	b9409be0 	ldr	w0, [sp, #152]
   80a9c:	34000080 	cbz	w0, 80aac <sort_reserve+0x67c>
   80aa0:	d0000020 	adrp	x0, 86000 <sa>
   80aa4:	91000000 	add	x0, x0, #0x0
   80aa8:	f9000c07 	str	x7, [x0, #24]
   80aac:	3400008a 	cbz	w10, 80abc <sort_reserve+0x68c>
   80ab0:	d0000020 	adrp	x0, 86000 <sa>
   80ab4:	91000000 	add	x0, x0, #0x0
   80ab8:	f9004c11 	str	x17, [x0, #152]
   80abc:	b9409fe0 	ldr	w0, [sp, #156]
   80ac0:	34000080 	cbz	w0, 80ad0 <sort_reserve+0x6a0>
   80ac4:	d0000020 	adrp	x0, 86000 <sa>
   80ac8:	91000000 	add	x0, x0, #0x0
   80acc:	f9001009 	str	x9, [x0, #32]
   80ad0:	3400008f 	cbz	w15, 80ae0 <sort_reserve+0x6b0>
   80ad4:	d0000020 	adrp	x0, 86000 <sa>
   80ad8:	91000000 	add	x0, x0, #0x0
   80adc:	f9005013 	str	x19, [x0, #160]
   80ae0:	b940cbe0 	ldr	w0, [sp, #200]
   80ae4:	34000080 	cbz	w0, 80af4 <sort_reserve+0x6c4>
   80ae8:	d0000020 	adrp	x0, 86000 <sa>
   80aec:	91000000 	add	x0, x0, #0x0
   80af0:	f900140b 	str	x11, [x0, #40]
   80af4:	34000092 	cbz	w18, 80b04 <sort_reserve+0x6d4>
   80af8:	d0000020 	adrp	x0, 86000 <sa>
   80afc:	91000000 	add	x0, x0, #0x0
   80b00:	f9005416 	str	x22, [x0, #168]
   80b04:	b940cfe0 	ldr	w0, [sp, #204]
   80b08:	34000080 	cbz	w0, 80b18 <sort_reserve+0x6e8>
   80b0c:	d0000020 	adrp	x0, 86000 <sa>
   80b10:	91000000 	add	x0, x0, #0x0
   80b14:	f9001810 	str	x16, [x0, #48]
   80b18:	34000094 	cbz	w20, 80b28 <sort_reserve+0x6f8>
   80b1c:	d0000020 	adrp	x0, 86000 <sa>
   80b20:	91000000 	add	x0, x0, #0x0
   80b24:	f9005819 	str	x25, [x0, #176]
   80b28:	b940ebe0 	ldr	w0, [sp, #232]
   80b2c:	34000080 	cbz	w0, 80b3c <sort_reserve+0x70c>
   80b30:	d0000020 	adrp	x0, 86000 <sa>
   80b34:	91000000 	add	x0, x0, #0x0
   80b38:	f9001c1e 	str	x30, [x0, #56]
   80b3c:	34000097 	cbz	w23, 80b4c <sort_reserve+0x71c>
   80b40:	d0000020 	adrp	x0, 86000 <sa>
   80b44:	91000000 	add	x0, x0, #0x0
   80b48:	f9005c1c 	str	x28, [x0, #184]
   80b4c:	b940efe0 	ldr	w0, [sp, #236]
   80b50:	34000080 	cbz	w0, 80b60 <sort_reserve+0x730>
   80b54:	d0000020 	adrp	x0, 86000 <sa>
   80b58:	91000000 	add	x0, x0, #0x0
   80b5c:	f9002015 	str	x21, [x0, #64]
   80b60:	340000ba 	cbz	w26, 80b74 <sort_reserve+0x744>
   80b64:	d0000020 	adrp	x0, 86000 <sa>
   80b68:	91000000 	add	x0, x0, #0x0
   80b6c:	f94037e1 	ldr	x1, [sp, #104]
   80b70:	f9006001 	str	x1, [x0, #192]
   80b74:	b940fbe0 	ldr	w0, [sp, #248]
   80b78:	34000080 	cbz	w0, 80b88 <sort_reserve+0x758>
   80b7c:	d0000020 	adrp	x0, 86000 <sa>
   80b80:	91000000 	add	x0, x0, #0x0
   80b84:	f9002418 	str	x24, [x0, #72]
   80b88:	b940ffe0 	ldr	w0, [sp, #252]
   80b8c:	340000a0 	cbz	w0, 80ba0 <sort_reserve+0x770>
   80b90:	d0000020 	adrp	x0, 86000 <sa>
   80b94:	91000000 	add	x0, x0, #0x0
   80b98:	f9403fe1 	ldr	x1, [sp, #120]
   80b9c:	f9006401 	str	x1, [x0, #200]
   80ba0:	b94103e0 	ldr	w0, [sp, #256]
   80ba4:	34000080 	cbz	w0, 80bb4 <sort_reserve+0x784>
   80ba8:	d0000020 	adrp	x0, 86000 <sa>
   80bac:	91000000 	add	x0, x0, #0x0
   80bb0:	f900281b 	str	x27, [x0, #80]
   80bb4:	b94107e0 	ldr	w0, [sp, #260]
   80bb8:	340000a0 	cbz	w0, 80bcc <sort_reserve+0x79c>
   80bbc:	d0000020 	adrp	x0, 86000 <sa>
   80bc0:	91000000 	add	x0, x0, #0x0
   80bc4:	f94047e1 	ldr	x1, [sp, #136]
   80bc8:	f9006801 	str	x1, [x0, #208]
   80bcc:	b9410fe0 	ldr	w0, [sp, #268]
   80bd0:	340000a0 	cbz	w0, 80be4 <sort_reserve+0x7b4>
   80bd4:	d0000020 	adrp	x0, 86000 <sa>
   80bd8:	91000000 	add	x0, x0, #0x0
   80bdc:	f94043e1 	ldr	x1, [sp, #128]
   80be0:	f9002c01 	str	x1, [x0, #88]
   80be4:	b94113e0 	ldr	w0, [sp, #272]
   80be8:	340000a0 	cbz	w0, 80bfc <sort_reserve+0x7cc>
   80bec:	d0000020 	adrp	x0, 86000 <sa>
   80bf0:	91000000 	add	x0, x0, #0x0
   80bf4:	f94053e1 	ldr	x1, [sp, #160]
   80bf8:	f9006c01 	str	x1, [x0, #216]
   80bfc:	b94117e0 	ldr	w0, [sp, #276]
   80c00:	340000a0 	cbz	w0, 80c14 <sort_reserve+0x7e4>
   80c04:	d0000020 	adrp	x0, 86000 <sa>
   80c08:	91000000 	add	x0, x0, #0x0
   80c0c:	f9404be1 	ldr	x1, [sp, #144]
   80c10:	f9003001 	str	x1, [x0, #96]
   80c14:	b9411be0 	ldr	w0, [sp, #280]
   80c18:	340000a0 	cbz	w0, 80c2c <sort_reserve+0x7fc>
   80c1c:	d0000020 	adrp	x0, 86000 <sa>
   80c20:	91000000 	add	x0, x0, #0x0
   80c24:	f9405fe1 	ldr	x1, [sp, #184]
   80c28:	f9007001 	str	x1, [x0, #224]
   80c2c:	b94123e0 	ldr	w0, [sp, #288]
   80c30:	340000a0 	cbz	w0, 80c44 <sort_reserve+0x814>
   80c34:	d0000020 	adrp	x0, 86000 <sa>
   80c38:	91000000 	add	x0, x0, #0x0
   80c3c:	f9405be1 	ldr	x1, [sp, #176]
   80c40:	f9003401 	str	x1, [x0, #104]
   80c44:	b94127e0 	ldr	w0, [sp, #292]
   80c48:	340000a0 	cbz	w0, 80c5c <sort_reserve+0x82c>
   80c4c:	d0000020 	adrp	x0, 86000 <sa>
   80c50:	91000000 	add	x0, x0, #0x0
   80c54:	f9406fe1 	ldr	x1, [sp, #216]
   80c58:	f9007401 	str	x1, [x0, #232]
   80c5c:	a94153f3 	ldp	x19, x20, [sp, #16]
   80c60:	a9425bf5 	ldp	x21, x22, [sp, #32]
   80c64:	a94363f7 	ldp	x23, x24, [sp, #48]
   80c68:	a9446bf9 	ldp	x25, x26, [sp, #64]
   80c6c:	a94573fb 	ldp	x27, x28, [sp, #80]
   80c70:	a8d37bfd 	ldp	x29, x30, [sp], #304
   80c74:	d65f03c0 	ret
   80c78:	f94073fb 	ldr	x27, [sp, #224]
   80c7c:	f9006ffb 	str	x27, [sp, #216]
   80c80:	f94057fb 	ldr	x27, [sp, #168]
   80c84:	f9005bfb 	str	x27, [sp, #176]
   80c88:	5280003b 	mov	w27, #0x1                   	// #1
   80c8c:	b9010bfb 	str	w27, [sp, #264]
   80c90:	b9011ffb 	str	w27, [sp, #284]
   80c94:	7100383f 	cmp	w1, #0xe
   80c98:	b90123fb 	str	w27, [sp, #288]
   80c9c:	b90127fb 	str	w27, [sp, #292]
   80ca0:	54ffd461 	b.ne	8072c <sort_reserve+0x2fc>  // b.any
   80ca4:	f90057e0 	str	x0, [sp, #168]
   80ca8:	f90073e2 	str	x2, [sp, #224]
   80cac:	17fffea9 	b	80750 <sort_reserve+0x320>
   80cb0:	f9407bfb 	ldr	x27, [sp, #240]
   80cb4:	f90073fb 	str	x27, [sp, #224]
   80cb8:	f94063fb 	ldr	x27, [sp, #192]
   80cbc:	f90057fb 	str	x27, [sp, #168]
   80cc0:	5280003b 	mov	w27, #0x1                   	// #1
   80cc4:	b9010bfb 	str	w27, [sp, #264]
   80cc8:	b9011ffb 	str	w27, [sp, #284]
   80ccc:	b9012bfb 	str	w27, [sp, #296]
   80cd0:	b9012ffb 	str	w27, [sp, #300]
   80cd4:	17fffe9d 	b	80748 <sort_reserve+0x318>
   80cd8:	d65f03c0 	ret
   80cdc:	d503201f 	nop

0000000000080ce0 <check_collision>:
   80ce0:	8b010001 	add	x1, x0, x1
   80ce4:	aa0003e4 	mov	x4, x0
   80ce8:	eb02003f 	cmp	x1, x2
   80cec:	1a9f97e5 	cset	w5, hi  // hi = pmore
   80cf0:	710000bf 	cmp	w5, #0x0
   80cf4:	fa421002 	ccmp	x0, x2, #0x2, ne  // ne = any
   80cf8:	52800020 	mov	w0, #0x1                   	// #1
   80cfc:	540000e9 	b.ls	80d18 <check_collision+0x38>  // b.plast
   80d00:	8b030043 	add	x3, x2, x3
   80d04:	52800020 	mov	w0, #0x1                   	// #1
   80d08:	eb03003f 	cmp	x1, x3
   80d0c:	fa423082 	ccmp	x4, x2, #0x2, cc  // cc = lo, ul, last
   80d10:	fa432082 	ccmp	x4, x3, #0x2, cs  // cs = hs, nlast
   80d14:	54000042 	b.cs	80d1c <check_collision+0x3c>  // b.hs, b.nlast
   80d18:	d65f03c0 	ret
   80d1c:	710000bf 	cmp	w5, #0x0
   80d20:	fa431022 	ccmp	x1, x3, #0x2, ne  // ne = any
   80d24:	1a9f87e0 	cset	w0, ls  // ls = plast
   80d28:	d65f03c0 	ret
   80d2c:	d503201f 	nop

0000000000080d30 <reserve_mem>:
   80d30:	aa0003e2 	mov	x2, x0
   80d34:	aa010000 	orr	x0, x0, x1
   80d38:	f2402c1f 	tst	x0, #0xfff
   80d3c:	540004a1 	b.ne	80dd0 <reserve_mem+0xa0>  // b.any
   80d40:	d0000028 	adrp	x8, 86000 <sa>
   80d44:	91000100 	add	x0, x8, #0x0
   80d48:	f9408005 	ldr	x5, [x0, #256]
   80d4c:	f1003cbf 	cmp	x5, #0xf
   80d50:	54000448 	b.hi	80dd8 <reserve_mem+0xa8>  // b.pmore
   80d54:	b40002e5 	cbz	x5, 80db0 <reserve_mem+0x80>
   80d58:	8b010047 	add	x7, x2, x1
   80d5c:	8b050c09 	add	x9, x0, x5, lsl #3
   80d60:	f9400004 	ldr	x4, [x0]
   80d64:	f9404003 	ldr	x3, [x0, #128]
   80d68:	8b030083 	add	x3, x4, x3
   80d6c:	eb03005f 	cmp	x2, x3
   80d70:	1a9f27e6 	cset	w6, cc  // cc = lo, ul, last
   80d74:	710000df 	cmp	w6, #0x0
   80d78:	fa421082 	ccmp	x4, x2, #0x2, ne  // ne = any
   80d7c:	540000a9 	b.ls	80d90 <reserve_mem+0x60>  // b.plast
   80d80:	eb07007f 	cmp	x3, x7
   80d84:	fa423082 	ccmp	x4, x2, #0x2, cc  // cc = lo, ul, last
   80d88:	fa472082 	ccmp	x4, x7, #0x2, cs  // cs = hs, nlast
   80d8c:	54000062 	b.cs	80d98 <reserve_mem+0x68>  // b.hs, b.nlast
   80d90:	12800040 	mov	w0, #0xfffffffd            	// #-3
   80d94:	d65f03c0 	ret
   80d98:	710000df 	cmp	w6, #0x0
   80d9c:	91002000 	add	x0, x0, #0x8
   80da0:	fa471062 	ccmp	x3, x7, #0x2, ne  // ne = any
   80da4:	54ffff69 	b.ls	80d90 <reserve_mem+0x60>  // b.plast
   80da8:	eb00013f 	cmp	x9, x0
   80dac:	54fffda1 	b.ne	80d60 <reserve_mem+0x30>  // b.any
   80db0:	91000108 	add	x8, x8, #0x0
   80db4:	910040a4 	add	x4, x5, #0x10
   80db8:	910004a3 	add	x3, x5, #0x1
   80dbc:	52800000 	mov	w0, #0x0                   	// #0
   80dc0:	f8257902 	str	x2, [x8, x5, lsl #3]
   80dc4:	f8247901 	str	x1, [x8, x4, lsl #3]
   80dc8:	f9008103 	str	x3, [x8, #256]
   80dcc:	d65f03c0 	ret
   80dd0:	12800000 	mov	w0, #0xffffffff            	// #-1
   80dd4:	d65f03c0 	ret
   80dd8:	12800020 	mov	w0, #0xfffffffe            	// #-2
   80ddc:	d65f03c0 	ret

0000000000080de0 <reserve_alloc>:
   80de0:	d0000023 	adrp	x3, 86000 <sa>
   80de4:	9100006a 	add	x10, x3, #0x0
   80de8:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   80dec:	913ffc01 	add	x1, x0, #0xfff
   80df0:	910003fd 	mov	x29, sp
   80df4:	f9408140 	ldr	x0, [x10, #256]
   80df8:	f1003c1f 	cmp	x0, #0xf
   80dfc:	54000808 	b.hi	80efc <reserve_alloc+0x11c>  // b.pmore
   80e00:	9274cc21 	and	x1, x1, #0xfffffffffffff000
   80e04:	d2800000 	mov	x0, #0x0                   	// #0
   80e08:	97ffffca 	bl	80d30 <reserve_mem>
   80e0c:	31000c1f 	cmn	w0, #0x3
   80e10:	d2800000 	mov	x0, #0x0                   	// #0
   80e14:	54000621 	b.ne	80ed8 <reserve_alloc+0xf8>  // b.any
   80e18:	f9408149 	ldr	x9, [x10, #256]
   80e1c:	b4000629 	cbz	x9, 80ee0 <reserve_alloc+0x100>
   80e20:	f1003d3f 	cmp	x9, #0xf
   80e24:	aa0a03e3 	mov	x3, x10
   80e28:	d2800002 	mov	x2, #0x0                   	// #0
   80e2c:	1a9f97ea 	cset	w10, hi  // hi = pmore
   80e30:	14000004 	b	80e40 <reserve_alloc+0x60>
   80e34:	91000442 	add	x2, x2, #0x1
   80e38:	eb02013f 	cmp	x9, x2
   80e3c:	54000520 	b.eq	80ee0 <reserve_alloc+0x100>  // b.none
   80e40:	8b020c64 	add	x4, x3, x2, lsl #3
   80e44:	93407c4b 	sxtw	x11, w2
   80e48:	f8627860 	ldr	x0, [x3, x2, lsl #3]
   80e4c:	f9404084 	ldr	x4, [x4, #128]
   80e50:	8b040000 	add	x0, x0, x4
   80e54:	f2402c1f 	tst	x0, #0xfff
   80e58:	7a400940 	ccmp	w10, #0x0, #0x0, eq  // eq = none
   80e5c:	540003e1 	b.ne	80ed8 <reserve_alloc+0xf8>  // b.any
   80e60:	8b000028 	add	x8, x1, x0
   80e64:	d2800006 	mov	x6, #0x0                   	// #0
   80e68:	8b060c64 	add	x4, x3, x6, lsl #3
   80e6c:	f8667865 	ldr	x5, [x3, x6, lsl #3]
   80e70:	f9404084 	ldr	x4, [x4, #128]
   80e74:	8b0400a4 	add	x4, x5, x4
   80e78:	eb04001f 	cmp	x0, x4
   80e7c:	1a9f27e7 	cset	w7, cc  // cc = lo, ul, last
   80e80:	710000ff 	cmp	w7, #0x0
   80e84:	fa451000 	ccmp	x0, x5, #0x0, ne  // ne = any
   80e88:	54fffd62 	b.cs	80e34 <reserve_alloc+0x54>  // b.hs, b.nlast
   80e8c:	eb08009f 	cmp	x4, x8
   80e90:	fa453000 	ccmp	x0, x5, #0x0, cc  // cc = lo, ul, last
   80e94:	fa4890a2 	ccmp	x5, x8, #0x2, ls  // ls = plast
   80e98:	54fffce3 	b.cc	80e34 <reserve_alloc+0x54>  // b.lo, b.ul, b.last
   80e9c:	710000ff 	cmp	w7, #0x0
   80ea0:	910004c6 	add	x6, x6, #0x1
   80ea4:	fa481082 	ccmp	x4, x8, #0x2, ne  // ne = any
   80ea8:	54fffc69 	b.ls	80e34 <reserve_alloc+0x54>  // b.plast
   80eac:	eb06013f 	cmp	x9, x6
   80eb0:	54fffdc1 	b.ne	80e68 <reserve_alloc+0x88>  // b.any
   80eb4:	91004124 	add	x4, x9, #0x10
   80eb8:	91004162 	add	x2, x11, #0x10
   80ebc:	f8297860 	str	x0, [x3, x9, lsl #3]
   80ec0:	91000529 	add	x9, x9, #0x1
   80ec4:	f9008069 	str	x9, [x3, #256]
   80ec8:	f8247861 	str	x1, [x3, x4, lsl #3]
   80ecc:	f86b7860 	ldr	x0, [x3, x11, lsl #3]
   80ed0:	f8627861 	ldr	x1, [x3, x2, lsl #3]
   80ed4:	8b010000 	add	x0, x0, x1
   80ed8:	a8c17bfd 	ldp	x29, x30, [sp], #16
   80edc:	d65f03c0 	ret
   80ee0:	52800021 	mov	w1, #0x1                   	// #1
   80ee4:	b0000020 	adrp	x0, 85000 <el1_evt+0x1000>
   80ee8:	91006000 	add	x0, x0, #0x18
   80eec:	94000871 	bl	830b0 <log>
   80ef0:	d2800000 	mov	x0, #0x0                   	// #0
   80ef4:	a8c17bfd 	ldp	x29, x30, [sp], #16
   80ef8:	d65f03c0 	ret
   80efc:	52800021 	mov	w1, #0x1                   	// #1
   80f00:	b0000020 	adrp	x0, 85000 <el1_evt+0x1000>
   80f04:	91000000 	add	x0, x0, #0x0
   80f08:	9400086a 	bl	830b0 <log>
   80f0c:	d2800000 	mov	x0, #0x0                   	// #0
   80f10:	a8c17bfd 	ldp	x29, x30, [sp], #16
   80f14:	d65f03c0 	ret

0000000000080f18 <set_buddy>:
   80f18:	b0000023 	adrp	x3, 85000 <el1_evt+0x1000>
   80f1c:	d34cfc05 	lsr	x5, x0, #12
   80f20:	f9417463 	ldr	x3, [x3, #744]
   80f24:	f9408066 	ldr	x6, [x3, #256]
   80f28:	386568c4 	ldrb	w4, [x6, x5]
   80f2c:	12001084 	and	w4, w4, #0x1f
   80f30:	2a020084 	orr	w4, w4, w2
   80f34:	382568c4 	strb	w4, [x6, x5]
   80f38:	f9408066 	ldr	x6, [x3, #256]
   80f3c:	386568c4 	ldrb	w4, [x6, x5]
   80f40:	121b6884 	and	w4, w4, #0xffffffe0
   80f44:	2a010084 	orr	w4, w4, w1
   80f48:	382568c4 	strb	w4, [x6, x5]
   80f4c:	35000122 	cbnz	w2, 80f70 <set_buddy+0x58>
   80f50:	937c7c21 	sbfiz	x1, x1, #4, #32
   80f54:	8b030022 	add	x2, x1, x3
   80f58:	f9000402 	str	x2, [x0, #8]
   80f5c:	f8636822 	ldr	x2, [x1, x3]
   80f60:	f9000002 	str	x2, [x0]
   80f64:	f8636822 	ldr	x2, [x1, x3]
   80f68:	f9000440 	str	x0, [x2, #8]
   80f6c:	f8236820 	str	x0, [x1, x3]
   80f70:	d65f03c0 	ret
   80f74:	d503201f 	nop

0000000000080f78 <place_buddy>:
   80f78:	b0000025 	adrp	x5, 85000 <el1_evt+0x1000>
   80f7c:	13001c4c 	sxtb	w12, w2
   80f80:	eb01001f 	cmp	x0, x1
   80f84:	528001eb 	mov	w11, #0xf                   	// #15
   80f88:	f94174a5 	ldr	x5, [x5, #744]
   80f8c:	5280002a 	mov	w10, #0x1                   	// #1
   80f90:	54000500 	b.eq	81030 <place_buddy+0xb8>  // b.none
   80f94:	d503201f 	nop
   80f98:	dac00004 	rbit	x4, x0
   80f9c:	dac00023 	rbit	x3, x1
   80fa0:	dac01084 	clz	x4, x4
   80fa4:	dac01063 	clz	x3, x3
   80fa8:	51003084 	sub	w4, w4, #0xc
   80fac:	51003063 	sub	w3, w3, #0xc
   80fb0:	71003c9f 	cmp	w4, #0xf
   80fb4:	d34cfc09 	lsr	x9, x0, #12
   80fb8:	1a8bd084 	csel	w4, w4, w11, le
   80fbc:	71003c7f 	cmp	w3, #0xf
   80fc0:	1a8bd063 	csel	w3, w3, w11, le
   80fc4:	11003087 	add	w7, w4, #0xc
   80fc8:	11003066 	add	w6, w3, #0xc
   80fcc:	6b03009f 	cmp	w4, w3
   80fd0:	1ac72147 	lsl	w7, w10, w7
   80fd4:	1ac62146 	lsl	w6, w10, w6
   80fd8:	f94080a8 	ldr	x8, [x5, #256]
   80fdc:	540002cc 	b.gt	81034 <place_buddy+0xbc>
   80fe0:	38696903 	ldrb	w3, [x8, x9]
   80fe4:	937c7c86 	sbfiz	x6, x4, #4, #32
   80fe8:	8b0500cd 	add	x13, x6, x5
   80fec:	12001063 	and	w3, w3, #0x1f
   80ff0:	2a030183 	orr	w3, w12, w3
   80ff4:	38296903 	strb	w3, [x8, x9]
   80ff8:	f94080a8 	ldr	x8, [x5, #256]
   80ffc:	38696903 	ldrb	w3, [x8, x9]
   81000:	121b6863 	and	w3, w3, #0xffffffe0
   81004:	2a040064 	orr	w4, w3, w4
   81008:	38296904 	strb	w4, [x8, x9]
   8100c:	350000c2 	cbnz	w2, 81024 <place_buddy+0xac>
   81010:	f86568c3 	ldr	x3, [x6, x5]
   81014:	a9003403 	stp	x3, x13, [x0]
   81018:	f86568c3 	ldr	x3, [x6, x5]
   8101c:	f9000460 	str	x0, [x3, #8]
   81020:	f82568c0 	str	x0, [x6, x5]
   81024:	8b27c000 	add	x0, x0, w7, sxtw
   81028:	eb00003f 	cmp	x1, x0
   8102c:	54fffb61 	b.ne	80f98 <place_buddy+0x20>  // b.any
   81030:	d65f03c0 	ret
   81034:	cb26c021 	sub	x1, x1, w6, sxtw
   81038:	937c7c67 	sbfiz	x7, x3, #4, #32
   8103c:	8b0500e9 	add	x9, x7, x5
   81040:	d34cfc26 	lsr	x6, x1, #12
   81044:	38666904 	ldrb	w4, [x8, x6]
   81048:	12001084 	and	w4, w4, #0x1f
   8104c:	2a040184 	orr	w4, w12, w4
   81050:	38266904 	strb	w4, [x8, x6]
   81054:	f94080a8 	ldr	x8, [x5, #256]
   81058:	38666904 	ldrb	w4, [x8, x6]
   8105c:	121b6884 	and	w4, w4, #0xffffffe0
   81060:	2a030083 	orr	w3, w4, w3
   81064:	38266903 	strb	w3, [x8, x6]
   81068:	35fffe02 	cbnz	w2, 81028 <place_buddy+0xb0>
   8106c:	f86568e3 	ldr	x3, [x7, x5]
   81070:	a9002423 	stp	x3, x9, [x1]
   81074:	f86568e3 	ldr	x3, [x7, x5]
   81078:	f9000461 	str	x1, [x3, #8]
   8107c:	f82568e1 	str	x1, [x7, x5]
   81080:	17ffffea 	b	81028 <place_buddy+0xb0>
   81084:	d503201f 	nop

0000000000081088 <alloc_page>:
   81088:	a9bb7bfd 	stp	x29, x30, [sp, #-80]!
   8108c:	913ffc03 	add	x3, x0, #0xfff
   81090:	9274cc63 	and	x3, x3, #0xfffffffffffff000
   81094:	dac01061 	clz	x1, x3
   81098:	910003fd 	mov	x29, sp
   8109c:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   810a0:	a90153f3 	stp	x19, x20, [sp, #16]
   810a4:	52800673 	mov	w19, #0x33                  	// #51
   810a8:	4b010273 	sub	w19, w19, w1
   810ac:	11003261 	add	w1, w19, #0xc
   810b0:	52800034 	mov	w20, #0x1                   	// #1
   810b4:	9100c000 	add	x0, x0, #0x30
   810b8:	52800002 	mov	w2, #0x0                   	// #0
   810bc:	1ac12281 	lsl	w1, w20, w1
   810c0:	51000421 	sub	w1, w1, #0x1
   810c4:	a9046bf9 	stp	x25, x26, [sp, #64]
   810c8:	93407c21 	sxtw	x1, w1
   810cc:	ea03003f 	tst	x1, x3
   810d0:	9a9f07e1 	cset	x1, ne  // ne = any
   810d4:	8b33c033 	add	x19, x1, w19, sxtw
   810d8:	aa1303e1 	mov	x1, x19
   810dc:	94000905 	bl	834f0 <log_hex>
   810e0:	f1003e7f 	cmp	x19, #0xf
   810e4:	54001848 	b.hi	813ec <alloc_page+0x364>  // b.pmore
   810e8:	90000034 	adrp	x20, 85000 <el1_evt+0x1000>
   810ec:	d37cee61 	lsl	x1, x19, #4
   810f0:	f9417680 	ldr	x0, [x20, #744]
   810f4:	8b010002 	add	x2, x0, x1
   810f8:	f8616819 	ldr	x25, [x0, x1]
   810fc:	eb02033f 	cmp	x25, x2
   81100:	540018c1 	b.ne	81418 <alloc_page+0x390>  // b.any
   81104:	f1003e7f 	cmp	x19, #0xf
   81108:	91000661 	add	x1, x19, #0x1
   8110c:	54000e60 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81110:	d37cec22 	lsl	x2, x1, #4
   81114:	8b020003 	add	x3, x0, x2
   81118:	f8626819 	ldr	x25, [x0, x2]
   8111c:	eb03033f 	cmp	x25, x3
   81120:	54000f01 	b.ne	81300 <alloc_page+0x278>  // b.any
   81124:	f1003a7f 	cmp	x19, #0xe
   81128:	91000a61 	add	x1, x19, #0x2
   8112c:	54000d60 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81130:	d37cec22 	lsl	x2, x1, #4
   81134:	8b020003 	add	x3, x0, x2
   81138:	f8626819 	ldr	x25, [x0, x2]
   8113c:	eb03033f 	cmp	x25, x3
   81140:	54000e01 	b.ne	81300 <alloc_page+0x278>  // b.any
   81144:	f100367f 	cmp	x19, #0xd
   81148:	91000e61 	add	x1, x19, #0x3
   8114c:	54000c60 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81150:	d37cec22 	lsl	x2, x1, #4
   81154:	8b020003 	add	x3, x0, x2
   81158:	f8626819 	ldr	x25, [x0, x2]
   8115c:	eb03033f 	cmp	x25, x3
   81160:	54000d01 	b.ne	81300 <alloc_page+0x278>  // b.any
   81164:	f100327f 	cmp	x19, #0xc
   81168:	91001261 	add	x1, x19, #0x4
   8116c:	54000b60 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81170:	d37cec22 	lsl	x2, x1, #4
   81174:	8b020003 	add	x3, x0, x2
   81178:	f8626819 	ldr	x25, [x0, x2]
   8117c:	eb03033f 	cmp	x25, x3
   81180:	54000c01 	b.ne	81300 <alloc_page+0x278>  // b.any
   81184:	f1002e7f 	cmp	x19, #0xb
   81188:	91001661 	add	x1, x19, #0x5
   8118c:	54000a60 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81190:	d37cec22 	lsl	x2, x1, #4
   81194:	8b020003 	add	x3, x0, x2
   81198:	f8626819 	ldr	x25, [x0, x2]
   8119c:	eb03033f 	cmp	x25, x3
   811a0:	54000b01 	b.ne	81300 <alloc_page+0x278>  // b.any
   811a4:	f1002a7f 	cmp	x19, #0xa
   811a8:	91001a61 	add	x1, x19, #0x6
   811ac:	54000960 	b.eq	812d8 <alloc_page+0x250>  // b.none
   811b0:	d37cec22 	lsl	x2, x1, #4
   811b4:	8b020003 	add	x3, x0, x2
   811b8:	f8626819 	ldr	x25, [x0, x2]
   811bc:	eb03033f 	cmp	x25, x3
   811c0:	54000a01 	b.ne	81300 <alloc_page+0x278>  // b.any
   811c4:	f100267f 	cmp	x19, #0x9
   811c8:	91001e61 	add	x1, x19, #0x7
   811cc:	54000860 	b.eq	812d8 <alloc_page+0x250>  // b.none
   811d0:	d37cec22 	lsl	x2, x1, #4
   811d4:	8b020003 	add	x3, x0, x2
   811d8:	f8626819 	ldr	x25, [x0, x2]
   811dc:	eb03033f 	cmp	x25, x3
   811e0:	54000901 	b.ne	81300 <alloc_page+0x278>  // b.any
   811e4:	f100227f 	cmp	x19, #0x8
   811e8:	91002261 	add	x1, x19, #0x8
   811ec:	54000760 	b.eq	812d8 <alloc_page+0x250>  // b.none
   811f0:	d37cec22 	lsl	x2, x1, #4
   811f4:	8b020003 	add	x3, x0, x2
   811f8:	f8626819 	ldr	x25, [x0, x2]
   811fc:	eb03033f 	cmp	x25, x3
   81200:	54000801 	b.ne	81300 <alloc_page+0x278>  // b.any
   81204:	f1001e7f 	cmp	x19, #0x7
   81208:	91002661 	add	x1, x19, #0x9
   8120c:	54000660 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81210:	d37cec22 	lsl	x2, x1, #4
   81214:	8b020003 	add	x3, x0, x2
   81218:	f8626819 	ldr	x25, [x0, x2]
   8121c:	eb03033f 	cmp	x25, x3
   81220:	54000701 	b.ne	81300 <alloc_page+0x278>  // b.any
   81224:	f1001a7f 	cmp	x19, #0x6
   81228:	91002a61 	add	x1, x19, #0xa
   8122c:	54000560 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81230:	d37cec22 	lsl	x2, x1, #4
   81234:	8b020003 	add	x3, x0, x2
   81238:	f8626819 	ldr	x25, [x0, x2]
   8123c:	eb03033f 	cmp	x25, x3
   81240:	54000601 	b.ne	81300 <alloc_page+0x278>  // b.any
   81244:	f100167f 	cmp	x19, #0x5
   81248:	91002e61 	add	x1, x19, #0xb
   8124c:	54000460 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81250:	d37cec22 	lsl	x2, x1, #4
   81254:	8b020003 	add	x3, x0, x2
   81258:	f8626819 	ldr	x25, [x0, x2]
   8125c:	eb03033f 	cmp	x25, x3
   81260:	54000501 	b.ne	81300 <alloc_page+0x278>  // b.any
   81264:	f100127f 	cmp	x19, #0x4
   81268:	91003261 	add	x1, x19, #0xc
   8126c:	54000360 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81270:	d37cec22 	lsl	x2, x1, #4
   81274:	8b020003 	add	x3, x0, x2
   81278:	f8626819 	ldr	x25, [x0, x2]
   8127c:	eb03033f 	cmp	x25, x3
   81280:	54000401 	b.ne	81300 <alloc_page+0x278>  // b.any
   81284:	f1000e7f 	cmp	x19, #0x3
   81288:	91003661 	add	x1, x19, #0xd
   8128c:	54000260 	b.eq	812d8 <alloc_page+0x250>  // b.none
   81290:	d37cec22 	lsl	x2, x1, #4
   81294:	8b020003 	add	x3, x0, x2
   81298:	f8626819 	ldr	x25, [x0, x2]
   8129c:	eb03033f 	cmp	x25, x3
   812a0:	54000301 	b.ne	81300 <alloc_page+0x278>  // b.any
   812a4:	f1000a7f 	cmp	x19, #0x2
   812a8:	91003a61 	add	x1, x19, #0xe
   812ac:	54000160 	b.eq	812d8 <alloc_page+0x250>  // b.none
   812b0:	d37cec22 	lsl	x2, x1, #4
   812b4:	8b020003 	add	x3, x0, x2
   812b8:	f8626819 	ldr	x25, [x0, x2]
   812bc:	eb03033f 	cmp	x25, x3
   812c0:	54000201 	b.ne	81300 <alloc_page+0x278>  // b.any
   812c4:	f100067f 	cmp	x19, #0x1
   812c8:	54000080 	b.eq	812d8 <alloc_page+0x250>  // b.none
   812cc:	f84f0c19 	ldr	x25, [x0, #240]!
   812d0:	eb00033f 	cmp	x25, x0
   812d4:	540009c1 	b.ne	8140c <alloc_page+0x384>  // b.any
   812d8:	52800021 	mov	w1, #0x1                   	// #1
   812dc:	d2800019 	mov	x25, #0x0                   	// #0
   812e0:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   812e4:	91016000 	add	x0, x0, #0x58
   812e8:	94000772 	bl	830b0 <log>
   812ec:	aa1903e0 	mov	x0, x25
   812f0:	a94153f3 	ldp	x19, x20, [sp, #16]
   812f4:	a9446bf9 	ldp	x25, x26, [sp, #64]
   812f8:	a8c57bfd 	ldp	x29, x30, [sp], #80
   812fc:	d65f03c0 	ret
   81300:	a9025bf5 	stp	x21, x22, [sp, #32]
   81304:	d34cff36 	lsr	x22, x25, #12
   81308:	f9417694 	ldr	x20, [x20, #744]
   8130c:	eb01027f 	cmp	x19, x1
   81310:	a9400323 	ldp	x3, x0, [x25]
   81314:	f9408282 	ldr	x2, [x20, #256]
   81318:	f9000460 	str	x0, [x3, #8]
   8131c:	f9000003 	str	x3, [x0]
   81320:	38766840 	ldrb	w0, [x2, x22]
   81324:	12001000 	and	w0, w0, #0x1f
   81328:	321b0000 	orr	w0, w0, #0x20
   8132c:	38366840 	strb	w0, [x2, x22]
   81330:	f9408282 	ldr	x2, [x20, #256]
   81334:	38766840 	ldrb	w0, [x2, x22]
   81338:	121b6800 	and	w0, w0, #0xffffffe0
   8133c:	2a130000 	orr	w0, w0, w19
   81340:	38366840 	strb	w0, [x2, x22]
   81344:	54000602 	b.cs	81404 <alloc_page+0x37c>  // b.hs, b.nlast
   81348:	d1000435 	sub	x21, x1, #0x1
   8134c:	aa1403fa 	mov	x26, x20
   81350:	a90363f7 	stp	x23, x24, [sp, #48]
   81354:	90000037 	adrp	x23, 85000 <el1_evt+0x1000>
   81358:	8b151294 	add	x20, x20, x21, lsl #4
   8135c:	9101e2f7 	add	x23, x23, #0x78
   81360:	52800038 	mov	w24, #0x1                   	// #1
   81364:	d503201f 	nop
   81368:	aa1703e0 	mov	x0, x23
   8136c:	52800042 	mov	w2, #0x2                   	// #2
   81370:	94000860 	bl	834f0 <log_hex>
   81374:	1ad52302 	lsl	w2, w24, w21
   81378:	eb15027f 	cmp	x19, x21
   8137c:	93407c42 	sxtw	x2, w2
   81380:	aa1503e1 	mov	x1, x21
   81384:	f9408344 	ldr	x4, [x26, #256]
   81388:	ca160042 	eor	x2, x2, x22
   8138c:	d374cc40 	lsl	x0, x2, #12
   81390:	38626883 	ldrb	w3, [x4, x2]
   81394:	12001063 	and	w3, w3, #0x1f
   81398:	38226883 	strb	w3, [x4, x2]
   8139c:	f9408344 	ldr	x4, [x26, #256]
   813a0:	38626883 	ldrb	w3, [x4, x2]
   813a4:	121b6863 	and	w3, w3, #0xffffffe0
   813a8:	2a150063 	orr	w3, w3, w21
   813ac:	38226883 	strb	w3, [x4, x2]
   813b0:	f9000414 	str	x20, [x0, #8]
   813b4:	d10006b5 	sub	x21, x21, #0x1
   813b8:	f9400282 	ldr	x2, [x20]
   813bc:	f9000002 	str	x2, [x0]
   813c0:	f9400282 	ldr	x2, [x20]
   813c4:	f9000440 	str	x0, [x2, #8]
   813c8:	f81f0680 	str	x0, [x20], #-16
   813cc:	54fffce1 	b.ne	81368 <alloc_page+0x2e0>  // b.any
   813d0:	a9425bf5 	ldp	x21, x22, [sp, #32]
   813d4:	a94363f7 	ldp	x23, x24, [sp, #48]
   813d8:	aa1903e0 	mov	x0, x25
   813dc:	a94153f3 	ldp	x19, x20, [sp, #16]
   813e0:	a9446bf9 	ldp	x25, x26, [sp, #64]
   813e4:	a8c57bfd 	ldp	x29, x30, [sp], #80
   813e8:	d65f03c0 	ret
   813ec:	2a1403e1 	mov	w1, w20
   813f0:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   813f4:	d2800019 	mov	x25, #0x0                   	// #0
   813f8:	91012000 	add	x0, x0, #0x48
   813fc:	9400072d 	bl	830b0 <log>
   81400:	17fffff6 	b	813d8 <alloc_page+0x350>
   81404:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81408:	17fffff4 	b	813d8 <alloc_page+0x350>
   8140c:	d28001e1 	mov	x1, #0xf                   	// #15
   81410:	a9025bf5 	stp	x21, x22, [sp, #32]
   81414:	17ffffbc 	b	81304 <alloc_page+0x27c>
   81418:	aa1303e1 	mov	x1, x19
   8141c:	a9025bf5 	stp	x21, x22, [sp, #32]
   81420:	17ffffb9 	b	81304 <alloc_page+0x27c>
   81424:	d503201f 	nop

0000000000081428 <new_pd>:
   81428:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   8142c:	52800002 	mov	w2, #0x0                   	// #0
   81430:	910003fd 	mov	x29, sp
   81434:	a90153f3 	stp	x19, x20, [sp, #16]
   81438:	90000033 	adrp	x19, 85000 <el1_evt+0x1000>
   8143c:	52820014 	mov	w20, #0x1000                	// #4096
   81440:	a9025bf5 	stp	x21, x22, [sp, #32]
   81444:	aa0003f5 	mov	x21, x0
   81448:	aa1503e1 	mov	x1, x21
   8144c:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   81450:	91022000 	add	x0, x0, #0x88
   81454:	94000827 	bl	834f0 <log_hex>
   81458:	f9416660 	ldr	x0, [x19, #712]
   8145c:	f8757800 	ldr	x0, [x0, x21, lsl #3]
   81460:	f9401416 	ldr	x22, [x0, #40]
   81464:	1ad62294 	lsl	w20, w20, w22
   81468:	93407e94 	sxtw	x20, w20
   8146c:	aa1403e0 	mov	x0, x20
   81470:	97ffff06 	bl	81088 <alloc_page>
   81474:	90000026 	adrp	x6, 85000 <el1_evt+0x1000>
   81478:	d34cfc07 	lsr	x7, x0, #12
   8147c:	52800022 	mov	w2, #0x1                   	// #1
   81480:	f94174c6 	ldr	x6, [x6, #744]
   81484:	1ad62042 	lsl	w2, w2, w22
   81488:	7100045f 	cmp	w2, #0x1
   8148c:	f94080c3 	ldr	x3, [x6, #256]
   81490:	38676861 	ldrb	w1, [x3, x7]
   81494:	12001021 	and	w1, w1, #0x1f
   81498:	321a0021 	orr	w1, w1, #0x40
   8149c:	38276861 	strb	w1, [x3, x7]
   814a0:	5400018d 	b.le	814d0 <new_pd+0xa8>
   814a4:	51000844 	sub	w4, w2, #0x2
   814a8:	910004e2 	add	x2, x7, #0x1
   814ac:	91000884 	add	x4, x4, #0x2
   814b0:	8b070084 	add	x4, x4, x7
   814b4:	d503201f 	nop
   814b8:	f94080c3 	ldr	x3, [x6, #256]
   814bc:	38676865 	ldrb	w5, [x3, x7]
   814c0:	38226865 	strb	w5, [x3, x2]
   814c4:	91000442 	add	x2, x2, #0x1
   814c8:	eb02009f 	cmp	x4, x2
   814cc:	54ffff61 	b.ne	814b8 <new_pd+0x90>  // b.any
   814d0:	f9416673 	ldr	x19, [x19, #712]
   814d4:	d1008294 	sub	x20, x20, #0x20
   814d8:	91008003 	add	x3, x0, #0x20
   814dc:	f8757a62 	ldr	x2, [x19, x21, lsl #3]
   814e0:	f9400041 	ldr	x1, [x2]
   814e4:	a9007c01 	stp	x1, xzr, [x0]
   814e8:	f900081f 	str	xzr, [x0, #16]
   814ec:	f9401441 	ldr	x1, [x2, #40]
   814f0:	f9000040 	str	x0, [x2]
   814f4:	f9000c15 	str	x21, [x0, #24]
   814f8:	91000421 	add	x1, x1, #0x1
   814fc:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81500:	f9000843 	str	x3, [x2, #16]
   81504:	a9020454 	stp	x20, x1, [x2, #32]
   81508:	a94153f3 	ldp	x19, x20, [sp, #16]
   8150c:	a8c37bfd 	ldp	x29, x30, [sp], #48
   81510:	d65f03c0 	ret
   81514:	d503201f 	nop

0000000000081518 <pop_slab_cache>:
   81518:	f9400401 	ldr	x1, [x0, #8]
   8151c:	aa0003e2 	mov	x2, x0
   81520:	f9400420 	ldr	x0, [x1, #8]
   81524:	b4000120 	cbz	x0, 81548 <pop_slab_cache+0x30>
   81528:	f9400824 	ldr	x4, [x1, #16]
   8152c:	f9400c43 	ldr	x3, [x2, #24]
   81530:	d1000484 	sub	x4, x4, #0x1
   81534:	f9400005 	ldr	x5, [x0]
   81538:	a9009025 	stp	x5, x4, [x1, #8]
   8153c:	d1000463 	sub	x3, x3, #0x1
   81540:	f9000c43 	str	x3, [x2, #24]
   81544:	d65f03c0 	ret
   81548:	f9400041 	ldr	x1, [x2]
   8154c:	f9400420 	ldr	x0, [x1, #8]
   81550:	b50000a0 	cbnz	x0, 81564 <pop_slab_cache+0x4c>
   81554:	d503201f 	nop
   81558:	f9400021 	ldr	x1, [x1]
   8155c:	f9400420 	ldr	x0, [x1, #8]
   81560:	b4ffffc0 	cbz	x0, 81558 <pop_slab_cache+0x40>
   81564:	f9400824 	ldr	x4, [x1, #16]
   81568:	f9000441 	str	x1, [x2, #8]
   8156c:	f9400c43 	ldr	x3, [x2, #24]
   81570:	d1000484 	sub	x4, x4, #0x1
   81574:	f9400005 	ldr	x5, [x0]
   81578:	a9009025 	stp	x5, x4, [x1, #8]
   8157c:	d1000463 	sub	x3, x3, #0x1
   81580:	f9000c43 	str	x3, [x2, #24]
   81584:	d65f03c0 	ret

0000000000081588 <slice_slab_remain>:
   81588:	90000021 	adrp	x1, 85000 <el1_evt+0x1000>
   8158c:	91000402 	add	x2, x0, #0x1
   81590:	f9416421 	ldr	x1, [x1, #712]
   81594:	d37cec42 	lsl	x2, x2, #4
   81598:	f8607821 	ldr	x1, [x1, x0, lsl #3]
   8159c:	f9400820 	ldr	x0, [x1, #16]
   815a0:	f9401023 	ldr	x3, [x1, #32]
   815a4:	8b020004 	add	x4, x0, x2
   815a8:	f9000824 	str	x4, [x1, #16]
   815ac:	cb020062 	sub	x2, x3, x2
   815b0:	f9001022 	str	x2, [x1, #32]
   815b4:	d65f03c0 	ret

00000000000815b8 <alloc_slab>:
   815b8:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   815bc:	52800002 	mov	w2, #0x0                   	// #0
   815c0:	910003fd 	mov	x29, sp
   815c4:	a90153f3 	stp	x19, x20, [sp, #16]
   815c8:	aa0003f3 	mov	x19, x0
   815cc:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   815d0:	d344fe74 	lsr	x20, x19, #4
   815d4:	a9025bf5 	stp	x21, x22, [sp, #32]
   815d8:	90000035 	adrp	x21, 85000 <el1_evt+0x1000>
   815dc:	d1000694 	sub	x20, x20, #0x1
   815e0:	91026000 	add	x0, x0, #0x98
   815e4:	aa1403e1 	mov	x1, x20
   815e8:	940007c2 	bl	834f0 <log_hex>
   815ec:	f94166b6 	ldr	x22, [x21, #712]
   815f0:	f8747ac2 	ldr	x2, [x22, x20, lsl #3]
   815f4:	b4000462 	cbz	x2, 81680 <alloc_slab+0xc8>
   815f8:	f9400c43 	ldr	x3, [x2, #24]
   815fc:	b5000283 	cbnz	x3, 8164c <alloc_slab+0x94>
   81600:	f9401041 	ldr	x1, [x2, #32]
   81604:	eb13003f 	cmp	x1, x19
   81608:	54000163 	b.cc	81634 <alloc_slab+0x7c>  // b.lo, b.ul, b.last
   8160c:	f9400840 	ldr	x0, [x2, #16]
   81610:	927cee73 	and	x19, x19, #0xfffffffffffffff0
   81614:	cb130021 	sub	x1, x1, x19
   81618:	f9001041 	str	x1, [x2, #32]
   8161c:	8b130013 	add	x19, x0, x19
   81620:	f9000853 	str	x19, [x2, #16]
   81624:	a94153f3 	ldp	x19, x20, [sp, #16]
   81628:	a9425bf5 	ldp	x21, x22, [sp, #32]
   8162c:	a8c37bfd 	ldp	x29, x30, [sp], #48
   81630:	d65f03c0 	ret
   81634:	aa1403e0 	mov	x0, x20
   81638:	97ffff7c 	bl	81428 <new_pd>
   8163c:	f94166b5 	ldr	x21, [x21, #712]
   81640:	f8747aa2 	ldr	x2, [x21, x20, lsl #3]
   81644:	f9401041 	ldr	x1, [x2, #32]
   81648:	17fffff1 	b	8160c <alloc_slab+0x54>
   8164c:	f9400441 	ldr	x1, [x2, #8]
   81650:	f9400420 	ldr	x0, [x1, #8]
   81654:	b4000340 	cbz	x0, 816bc <alloc_slab+0x104>
   81658:	f9400824 	ldr	x4, [x1, #16]
   8165c:	d1000463 	sub	x3, x3, #0x1
   81660:	f9400005 	ldr	x5, [x0]
   81664:	d1000484 	sub	x4, x4, #0x1
   81668:	a9009025 	stp	x5, x4, [x1, #8]
   8166c:	f9000c43 	str	x3, [x2, #24]
   81670:	a94153f3 	ldp	x19, x20, [sp, #16]
   81674:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81678:	a8c37bfd 	ldp	x29, x30, [sp], #48
   8167c:	d65f03c0 	ret
   81680:	d2800600 	mov	x0, #0x30                  	// #48
   81684:	97ffffcd 	bl	815b8 <alloc_slab>
   81688:	aa0003e1 	mov	x1, x0
   8168c:	f8347ac1 	str	x1, [x22, x20, lsl #3]
   81690:	aa1403e0 	mov	x0, x20
   81694:	f900003f 	str	xzr, [x1]
   81698:	f9000c3f 	str	xzr, [x1, #24]
   8169c:	f900143f 	str	xzr, [x1, #40]
   816a0:	97ffff62 	bl	81428 <new_pd>
   816a4:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   816a8:	f8747ac2 	ldr	x2, [x22, x20, lsl #3]
   816ac:	f9416800 	ldr	x0, [x0, #720]
   816b0:	f9400000 	ldr	x0, [x0]
   816b4:	f9000440 	str	x0, [x2, #8]
   816b8:	17ffffd0 	b	815f8 <alloc_slab+0x40>
   816bc:	f9400041 	ldr	x1, [x2]
   816c0:	f9400420 	ldr	x0, [x1, #8]
   816c4:	b5000080 	cbnz	x0, 816d4 <alloc_slab+0x11c>
   816c8:	f9400021 	ldr	x1, [x1]
   816cc:	f9400420 	ldr	x0, [x1, #8]
   816d0:	b4ffffc0 	cbz	x0, 816c8 <alloc_slab+0x110>
   816d4:	f9000441 	str	x1, [x2, #8]
   816d8:	17ffffe0 	b	81658 <alloc_slab+0xa0>
   816dc:	d503201f 	nop

00000000000816e0 <free_page>:
   816e0:	a9bb7bfd 	stp	x29, x30, [sp, #-80]!
   816e4:	90000021 	adrp	x1, 85000 <el1_evt+0x1000>
   816e8:	52800002 	mov	w2, #0x0                   	// #0
   816ec:	910003fd 	mov	x29, sp
   816f0:	a90363f7 	stp	x23, x24, [sp, #48]
   816f4:	90000037 	adrp	x23, 85000 <el1_evt+0x1000>
   816f8:	a9025bf5 	stp	x21, x22, [sp, #32]
   816fc:	d34cfc15 	lsr	x21, x0, #12
   81700:	9102c020 	add	x0, x1, #0xb0
   81704:	aa1503e1 	mov	x1, x21
   81708:	a90153f3 	stp	x19, x20, [sp, #16]
   8170c:	f90023f9 	str	x25, [sp, #64]
   81710:	94000778 	bl	834f0 <log_hex>
   81714:	f94176f9 	ldr	x25, [x23, #744]
   81718:	52800401 	mov	w1, #0x20                  	// #32
   8171c:	f9408320 	ldr	x0, [x25, #256]
   81720:	38756814 	ldrb	w20, [x0, x21]
   81724:	38356801 	strb	w1, [x0, x21]
   81728:	12001280 	and	w0, w20, #0x1f
   8172c:	92401294 	and	x20, x20, #0x1f
   81730:	7100381f 	cmp	w0, #0xe
   81734:	54000748 	b.hi	8181c <free_page+0x13c>  // b.pmore
   81738:	90000036 	adrp	x22, 85000 <el1_evt+0x1000>
   8173c:	52800038 	mov	w24, #0x1                   	// #1
   81740:	910322d6 	add	x22, x22, #0xc8
   81744:	14000010 	b	81784 <free_page+0xa4>
   81748:	eb23029f 	cmp	x20, w3, uxtb
   8174c:	54000301 	b.ne	817ac <free_page+0xcc>  // b.any
   81750:	94000768 	bl	834f0 <log_hex>
   81754:	91000694 	add	x20, x20, #0x1
   81758:	d374ce60 	lsl	x0, x19, #12
   8175c:	52800403 	mov	w3, #0x20                  	// #32
   81760:	f9408322 	ldr	x2, [x25, #256]
   81764:	eb1302bf 	cmp	x21, x19
   81768:	a9400001 	ldp	x1, x0, [x0]
   8176c:	f9000420 	str	x0, [x1, #8]
   81770:	9a9392b5 	csel	x21, x21, x19, ls  // ls = plast
   81774:	f1003e9f 	cmp	x20, #0xf
   81778:	f9000001 	str	x1, [x0]
   8177c:	38336843 	strb	w3, [x2, x19]
   81780:	54000480 	b.eq	81810 <free_page+0x130>  // b.none
   81784:	1ad42313 	lsl	w19, w24, w20
   81788:	aa1403e1 	mov	x1, x20
   8178c:	93407e73 	sxtw	x19, w19
   81790:	aa1603e0 	mov	x0, x22
   81794:	f9408324 	ldr	x4, [x25, #256]
   81798:	ca150273 	eor	x19, x19, x21
   8179c:	52800042 	mov	w2, #0x2                   	// #2
   817a0:	38736883 	ldrb	w3, [x4, x19]
   817a4:	721b087f 	tst	w3, #0xe0
   817a8:	54fffd00 	b.eq	81748 <free_page+0x68>  // b.none
   817ac:	12001e80 	and	w0, w20, #0xff
   817b0:	38756882 	ldrb	w2, [x4, x21]
   817b4:	d374cea1 	lsl	x1, x21, #12
   817b8:	f94176f7 	ldr	x23, [x23, #744]
   817bc:	12001042 	and	w2, w2, #0x1f
   817c0:	38356882 	strb	w2, [x4, x21]
   817c4:	d37cee94 	lsl	x20, x20, #4
   817c8:	8b170285 	add	x5, x20, x23
   817cc:	f94082e3 	ldr	x3, [x23, #256]
   817d0:	38756862 	ldrb	w2, [x3, x21]
   817d4:	121b6842 	and	w2, w2, #0xffffffe0
   817d8:	2a020000 	orr	w0, w0, w2
   817dc:	38356860 	strb	w0, [x3, x21]
   817e0:	a9425bf5 	ldp	x21, x22, [sp, #32]
   817e4:	f94023f9 	ldr	x25, [sp, #64]
   817e8:	f9000425 	str	x5, [x1, #8]
   817ec:	f8776a80 	ldr	x0, [x20, x23]
   817f0:	f9000020 	str	x0, [x1]
   817f4:	f8776a80 	ldr	x0, [x20, x23]
   817f8:	f9000401 	str	x1, [x0, #8]
   817fc:	f8376a81 	str	x1, [x20, x23]
   81800:	a94153f3 	ldp	x19, x20, [sp, #16]
   81804:	a94363f7 	ldp	x23, x24, [sp, #48]
   81808:	a8c57bfd 	ldp	x29, x30, [sp], #80
   8180c:	d65f03c0 	ret
   81810:	2a1403e0 	mov	w0, w20
   81814:	f9408324 	ldr	x4, [x25, #256]
   81818:	17ffffe6 	b	817b0 <free_page+0xd0>
   8181c:	f9408324 	ldr	x4, [x25, #256]
   81820:	17ffffe4 	b	817b0 <free_page+0xd0>
   81824:	d503201f 	nop

0000000000081828 <free_reserve>:
   81828:	a9bb7bfd 	stp	x29, x30, [sp, #-80]!
   8182c:	90000022 	adrp	x2, 85000 <el1_evt+0x1000>
   81830:	910003fd 	mov	x29, sp
   81834:	a90153f3 	stp	x19, x20, [sp, #16]
   81838:	aa0003f4 	mov	x20, x0
   8183c:	aa1403e1 	mov	x1, x20
   81840:	91036040 	add	x0, x2, #0xd8
   81844:	52800002 	mov	w2, #0x0                   	// #0
   81848:	9400072a 	bl	834f0 <log_hex>
   8184c:	b0000020 	adrp	x0, 86000 <sa>
   81850:	91000002 	add	x2, x0, #0x0
   81854:	f9408041 	ldr	x1, [x2, #256]
   81858:	b4000fe1 	cbz	x1, 81a54 <free_reserve+0x22c>
   8185c:	f9400003 	ldr	x3, [x0]
   81860:	a9025bf5 	stp	x21, x22, [sp, #32]
   81864:	eb03029f 	cmp	x20, x3
   81868:	54001140 	b.eq	81a90 <free_reserve+0x268>  // b.none
   8186c:	f100043f 	cmp	x1, #0x1
   81870:	aa1403f5 	mov	x21, x20
   81874:	54001769 	b.ls	81b60 <free_reserve+0x338>  // b.plast
   81878:	91000002 	add	x2, x0, #0x0
   8187c:	f9400443 	ldr	x3, [x2, #8]
   81880:	eb03029f 	cmp	x20, x3
   81884:	54001160 	b.eq	81ab0 <free_reserve+0x288>  // b.none
   81888:	f100083f 	cmp	x1, #0x2
   8188c:	54000a60 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81890:	91000002 	add	x2, x0, #0x0
   81894:	f9400843 	ldr	x3, [x2, #16]
   81898:	eb03029f 	cmp	x20, x3
   8189c:	54000f40 	b.eq	81a84 <free_reserve+0x25c>  // b.none
   818a0:	f1000c3f 	cmp	x1, #0x3
   818a4:	540009a0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   818a8:	91000002 	add	x2, x0, #0x0
   818ac:	f9400c43 	ldr	x3, [x2, #24]
   818b0:	eb14007f 	cmp	x3, x20
   818b4:	54001040 	b.eq	81abc <free_reserve+0x294>  // b.none
   818b8:	f100103f 	cmp	x1, #0x4
   818bc:	540008e0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   818c0:	91000002 	add	x2, x0, #0x0
   818c4:	f9401043 	ldr	x3, [x2, #32]
   818c8:	eb03029f 	cmp	x20, x3
   818cc:	54000fe0 	b.eq	81ac8 <free_reserve+0x2a0>  // b.none
   818d0:	f100143f 	cmp	x1, #0x5
   818d4:	54000820 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   818d8:	91000002 	add	x2, x0, #0x0
   818dc:	f9401443 	ldr	x3, [x2, #40]
   818e0:	eb03029f 	cmp	x20, x3
   818e4:	540010e0 	b.eq	81b00 <free_reserve+0x2d8>  // b.none
   818e8:	f100183f 	cmp	x1, #0x6
   818ec:	54000760 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   818f0:	91000002 	add	x2, x0, #0x0
   818f4:	f9401843 	ldr	x3, [x2, #48]
   818f8:	eb03029f 	cmp	x20, x3
   818fc:	54001080 	b.eq	81b0c <free_reserve+0x2e4>  // b.none
   81900:	f1001c3f 	cmp	x1, #0x7
   81904:	540006a0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81908:	91000002 	add	x2, x0, #0x0
   8190c:	f9401c43 	ldr	x3, [x2, #56]
   81910:	eb03029f 	cmp	x20, x3
   81914:	54001020 	b.eq	81b18 <free_reserve+0x2f0>  // b.none
   81918:	f100203f 	cmp	x1, #0x8
   8191c:	540005e0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81920:	91000002 	add	x2, x0, #0x0
   81924:	f9402043 	ldr	x3, [x2, #64]
   81928:	eb03029f 	cmp	x20, x3
   8192c:	54000fc0 	b.eq	81b24 <free_reserve+0x2fc>  // b.none
   81930:	f100243f 	cmp	x1, #0x9
   81934:	54000520 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81938:	91000002 	add	x2, x0, #0x0
   8193c:	f9402443 	ldr	x3, [x2, #72]
   81940:	eb03029f 	cmp	x20, x3
   81944:	54000f60 	b.eq	81b30 <free_reserve+0x308>  // b.none
   81948:	f100283f 	cmp	x1, #0xa
   8194c:	54000460 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81950:	91000002 	add	x2, x0, #0x0
   81954:	f9402843 	ldr	x3, [x2, #80]
   81958:	eb03029f 	cmp	x20, x3
   8195c:	54000c00 	b.eq	81adc <free_reserve+0x2b4>  // b.none
   81960:	f1002c3f 	cmp	x1, #0xb
   81964:	540003a0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81968:	91000002 	add	x2, x0, #0x0
   8196c:	f9402c43 	ldr	x3, [x2, #88]
   81970:	eb03029f 	cmp	x20, x3
   81974:	54000ba0 	b.eq	81ae8 <free_reserve+0x2c0>  // b.none
   81978:	f100303f 	cmp	x1, #0xc
   8197c:	540002e0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81980:	91000002 	add	x2, x0, #0x0
   81984:	f9403043 	ldr	x3, [x2, #96]
   81988:	eb03029f 	cmp	x20, x3
   8198c:	54000b40 	b.eq	81af4 <free_reserve+0x2cc>  // b.none
   81990:	f100343f 	cmp	x1, #0xd
   81994:	54000220 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   81998:	91000002 	add	x2, x0, #0x0
   8199c:	f9403443 	ldr	x3, [x2, #104]
   819a0:	eb03029f 	cmp	x20, x3
   819a4:	54000cc0 	b.eq	81b3c <free_reserve+0x314>  // b.none
   819a8:	f100383f 	cmp	x1, #0xe
   819ac:	54000160 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   819b0:	91000002 	add	x2, x0, #0x0
   819b4:	f9403843 	ldr	x3, [x2, #112]
   819b8:	eb03029f 	cmp	x20, x3
   819bc:	54000c60 	b.eq	81b48 <free_reserve+0x320>  // b.none
   819c0:	f1003c3f 	cmp	x1, #0xf
   819c4:	540000a0 	b.eq	819d8 <free_reserve+0x1b0>  // b.none
   819c8:	91000000 	add	x0, x0, #0x0
   819cc:	f9403c01 	ldr	x1, [x0, #120]
   819d0:	eb01029f 	cmp	x20, x1
   819d4:	54000c00 	b.eq	81b54 <free_reserve+0x32c>  // b.none
   819d8:	eb15029f 	cmp	x20, x21
   819dc:	54000c20 	b.eq	81b60 <free_reserve+0x338>  // b.none
   819e0:	dac002b6 	rbit	x22, x21
   819e4:	f90023f9 	str	x25, [sp, #64]
   819e8:	dac00299 	rbit	x25, x20
   819ec:	dac012d6 	clz	x22, x22
   819f0:	dac01339 	clz	x25, x25
   819f4:	a90363f7 	stp	x23, x24, [sp, #48]
   819f8:	528001f8 	mov	w24, #0xf                   	// #15
   819fc:	52800037 	mov	w23, #0x1                   	// #1
   81a00:	51003322 	sub	w2, w25, #0xc
   81a04:	510032c3 	sub	w3, w22, #0xc
   81a08:	71003c5f 	cmp	w2, #0xf
   81a0c:	aa1403e0 	mov	x0, x20
   81a10:	1a98d053 	csel	w19, w2, w24, le
   81a14:	71003c7f 	cmp	w3, #0xf
   81a18:	1a98d062 	csel	w2, w3, w24, le
   81a1c:	11003041 	add	w1, w2, #0xc
   81a20:	6b02027f 	cmp	w19, w2
   81a24:	540001ec 	b.gt	81a60 <free_reserve+0x238>
   81a28:	97ffff2e 	bl	816e0 <free_page>
   81a2c:	11003262 	add	w2, w19, #0xc
   81a30:	1ac222e2 	lsl	w2, w23, w2
   81a34:	8b22c294 	add	x20, x20, w2, sxtw
   81a38:	dac00299 	rbit	x25, x20
   81a3c:	eb15029f 	cmp	x20, x21
   81a40:	dac01339 	clz	x25, x25
   81a44:	54fffde1 	b.ne	81a00 <free_reserve+0x1d8>  // b.any
   81a48:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81a4c:	a94363f7 	ldp	x23, x24, [sp, #48]
   81a50:	f94023f9 	ldr	x25, [sp, #64]
   81a54:	a94153f3 	ldp	x19, x20, [sp, #16]
   81a58:	a8c57bfd 	ldp	x29, x30, [sp], #80
   81a5c:	d65f03c0 	ret
   81a60:	1ac122e1 	lsl	w1, w23, w1
   81a64:	cb21c2b5 	sub	x21, x21, w1, sxtw
   81a68:	dac002b6 	rbit	x22, x21
   81a6c:	aa1503e0 	mov	x0, x21
   81a70:	97ffff1c 	bl	816e0 <free_page>
   81a74:	dac012d6 	clz	x22, x22
   81a78:	eb1402bf 	cmp	x21, x20
   81a7c:	54fffc21 	b.ne	81a00 <free_reserve+0x1d8>  // b.any
   81a80:	17fffff2 	b	81a48 <free_reserve+0x220>
   81a84:	f9404842 	ldr	x2, [x2, #144]
   81a88:	8b0202b5 	add	x21, x21, x2
   81a8c:	17ffff85 	b	818a0 <free_reserve+0x78>
   81a90:	f9404055 	ldr	x21, [x2, #128]
   81a94:	f100043f 	cmp	x1, #0x1
   81a98:	8b150295 	add	x21, x20, x21
   81a9c:	54fff9e9 	b.ls	819d8 <free_reserve+0x1b0>  // b.plast
   81aa0:	91000002 	add	x2, x0, #0x0
   81aa4:	f9400443 	ldr	x3, [x2, #8]
   81aa8:	eb03029f 	cmp	x20, x3
   81aac:	54ffeee1 	b.ne	81888 <free_reserve+0x60>  // b.any
   81ab0:	f9404442 	ldr	x2, [x2, #136]
   81ab4:	8b0202b5 	add	x21, x21, x2
   81ab8:	17ffff74 	b	81888 <free_reserve+0x60>
   81abc:	f9404c42 	ldr	x2, [x2, #152]
   81ac0:	8b0202b5 	add	x21, x21, x2
   81ac4:	17ffff7d 	b	818b8 <free_reserve+0x90>
   81ac8:	f9405042 	ldr	x2, [x2, #160]
   81acc:	f100143f 	cmp	x1, #0x5
   81ad0:	8b0202b5 	add	x21, x21, x2
   81ad4:	54fff021 	b.ne	818d8 <free_reserve+0xb0>  // b.any
   81ad8:	17ffffc0 	b	819d8 <free_reserve+0x1b0>
   81adc:	f9406842 	ldr	x2, [x2, #208]
   81ae0:	8b0202b5 	add	x21, x21, x2
   81ae4:	17ffff9f 	b	81960 <free_reserve+0x138>
   81ae8:	f9406c42 	ldr	x2, [x2, #216]
   81aec:	8b0202b5 	add	x21, x21, x2
   81af0:	17ffffa2 	b	81978 <free_reserve+0x150>
   81af4:	f9407042 	ldr	x2, [x2, #224]
   81af8:	8b0202b5 	add	x21, x21, x2
   81afc:	17ffffa5 	b	81990 <free_reserve+0x168>
   81b00:	f9405442 	ldr	x2, [x2, #168]
   81b04:	8b0202b5 	add	x21, x21, x2
   81b08:	17ffff78 	b	818e8 <free_reserve+0xc0>
   81b0c:	f9405842 	ldr	x2, [x2, #176]
   81b10:	8b0202b5 	add	x21, x21, x2
   81b14:	17ffff7b 	b	81900 <free_reserve+0xd8>
   81b18:	f9405c42 	ldr	x2, [x2, #184]
   81b1c:	8b0202b5 	add	x21, x21, x2
   81b20:	17ffff7e 	b	81918 <free_reserve+0xf0>
   81b24:	f9406042 	ldr	x2, [x2, #192]
   81b28:	8b0202b5 	add	x21, x21, x2
   81b2c:	17ffff81 	b	81930 <free_reserve+0x108>
   81b30:	f9406442 	ldr	x2, [x2, #200]
   81b34:	8b0202b5 	add	x21, x21, x2
   81b38:	17ffff84 	b	81948 <free_reserve+0x120>
   81b3c:	f9407442 	ldr	x2, [x2, #232]
   81b40:	8b0202b5 	add	x21, x21, x2
   81b44:	17ffff99 	b	819a8 <free_reserve+0x180>
   81b48:	f9407842 	ldr	x2, [x2, #240]
   81b4c:	8b0202b5 	add	x21, x21, x2
   81b50:	17ffff9c 	b	819c0 <free_reserve+0x198>
   81b54:	f9407c00 	ldr	x0, [x0, #248]
   81b58:	8b0002b5 	add	x21, x21, x0
   81b5c:	17ffff9f 	b	819d8 <free_reserve+0x1b0>
   81b60:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81b64:	17ffffbc 	b	81a54 <free_reserve+0x22c>

0000000000081b68 <free_slab>:
   81b68:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   81b6c:	90000021 	adrp	x1, 85000 <el1_evt+0x1000>
   81b70:	52800002 	mov	w2, #0x0                   	// #0
   81b74:	910003fd 	mov	x29, sp
   81b78:	f9000bf3 	str	x19, [sp, #16]
   81b7c:	aa0003f3 	mov	x19, x0
   81b80:	9103a020 	add	x0, x1, #0xe8
   81b84:	aa1303e1 	mov	x1, x19
   81b88:	9400065a 	bl	834f0 <log_hex>
   81b8c:	90000020 	adrp	x0, 85000 <el1_evt+0x1000>
   81b90:	d34cfe63 	lsr	x3, x19, #12
   81b94:	90000021 	adrp	x1, 85000 <el1_evt+0x1000>
   81b98:	f9417400 	ldr	x0, [x0, #744]
   81b9c:	f9416421 	ldr	x1, [x1, #712]
   81ba0:	f9408000 	ldr	x0, [x0, #256]
   81ba4:	38636802 	ldrb	w2, [x0, x3]
   81ba8:	12001040 	and	w0, w2, #0x1f
   81bac:	9ac02462 	lsr	x2, x3, x0
   81bb0:	9ac02042 	lsl	x2, x2, x0
   81bb4:	d374cc42 	lsl	x2, x2, #12
   81bb8:	f9400c40 	ldr	x0, [x2, #24]
   81bbc:	f9400443 	ldr	x3, [x2, #8]
   81bc0:	f8607820 	ldr	x0, [x1, x0, lsl #3]
   81bc4:	f9400404 	ldr	x4, [x0, #8]
   81bc8:	f9400c01 	ldr	x1, [x0, #24]
   81bcc:	f9000263 	str	x3, [x19]
   81bd0:	f9400843 	ldr	x3, [x2, #16]
   81bd4:	91000421 	add	x1, x1, #0x1
   81bd8:	91000463 	add	x3, x3, #0x1
   81bdc:	a9008c53 	stp	x19, x3, [x2, #8]
   81be0:	f9400483 	ldr	x3, [x4, #8]
   81be4:	f9000c01 	str	x1, [x0, #24]
   81be8:	b4000083 	cbz	x3, 81bf8 <free_slab+0x90>
   81bec:	f9400bf3 	ldr	x19, [sp, #16]
   81bf0:	a8c27bfd 	ldp	x29, x30, [sp], #32
   81bf4:	d65f03c0 	ret
   81bf8:	f9000402 	str	x2, [x0, #8]
   81bfc:	f9400bf3 	ldr	x19, [sp, #16]
   81c00:	a8c27bfd 	ldp	x29, x30, [sp], #32
   81c04:	d65f03c0 	ret

0000000000081c08 <kmalloc>:
   81c08:	a9bc7bfd 	stp	x29, x30, [sp, #-64]!
   81c0c:	910003fd 	mov	x29, sp
   81c10:	a90153f3 	stp	x19, x20, [sp, #16]
   81c14:	91003c13 	add	x19, x0, #0xf
   81c18:	927cee73 	and	x19, x19, #0xfffffffffffffff0
   81c1c:	f120027f 	cmp	x19, #0x800
   81c20:	540000a9 	b.ls	81c34 <kmalloc+0x2c>  // b.plast
   81c24:	aa1303e0 	mov	x0, x19
   81c28:	a94153f3 	ldp	x19, x20, [sp, #16]
   81c2c:	a8c47bfd 	ldp	x29, x30, [sp], #64
   81c30:	17fffd16 	b	81088 <alloc_page>
   81c34:	d344fe74 	lsr	x20, x19, #4
   81c38:	a9025bf5 	stp	x21, x22, [sp, #32]
   81c3c:	90000036 	adrp	x22, 85000 <el1_evt+0x1000>
   81c40:	d1000694 	sub	x20, x20, #0x1
   81c44:	90000035 	adrp	x21, 85000 <el1_evt+0x1000>
   81c48:	910262b5 	add	x21, x21, #0x98
   81c4c:	aa1403e1 	mov	x1, x20
   81c50:	aa1503e0 	mov	x0, x21
   81c54:	52800002 	mov	w2, #0x0                   	// #0
   81c58:	a90363f7 	stp	x23, x24, [sp, #48]
   81c5c:	94000625 	bl	834f0 <log_hex>
   81c60:	f94166d7 	ldr	x23, [x22, #712]
   81c64:	f8747ae1 	ldr	x1, [x23, x20, lsl #3]
   81c68:	b4000481 	cbz	x1, 81cf8 <kmalloc+0xf0>
   81c6c:	f9400c23 	ldr	x3, [x1, #24]
   81c70:	b50001c3 	cbnz	x3, 81ca8 <kmalloc+0xa0>
   81c74:	f9401020 	ldr	x0, [x1, #32]
   81c78:	eb00027f 	cmp	x19, x0
   81c7c:	54000328 	b.hi	81ce0 <kmalloc+0xd8>  // b.pmore
   81c80:	cb130002 	sub	x2, x0, x19
   81c84:	f9001022 	str	x2, [x1, #32]
   81c88:	f9400820 	ldr	x0, [x1, #16]
   81c8c:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81c90:	8b130013 	add	x19, x0, x19
   81c94:	f9000833 	str	x19, [x1, #16]
   81c98:	a94153f3 	ldp	x19, x20, [sp, #16]
   81c9c:	a94363f7 	ldp	x23, x24, [sp, #48]
   81ca0:	a8c47bfd 	ldp	x29, x30, [sp], #64
   81ca4:	d65f03c0 	ret
   81ca8:	f9400422 	ldr	x2, [x1, #8]
   81cac:	f9400440 	ldr	x0, [x2, #8]
   81cb0:	b40005e0 	cbz	x0, 81d6c <kmalloc+0x164>
   81cb4:	f9400844 	ldr	x4, [x2, #16]
   81cb8:	d1000463 	sub	x3, x3, #0x1
   81cbc:	f9400005 	ldr	x5, [x0]
   81cc0:	d1000484 	sub	x4, x4, #0x1
   81cc4:	a9009045 	stp	x5, x4, [x2, #8]
   81cc8:	f9000c23 	str	x3, [x1, #24]
   81ccc:	a94153f3 	ldp	x19, x20, [sp, #16]
   81cd0:	a9425bf5 	ldp	x21, x22, [sp, #32]
   81cd4:	a94363f7 	ldp	x23, x24, [sp, #48]
   81cd8:	a8c47bfd 	ldp	x29, x30, [sp], #64
   81cdc:	d65f03c0 	ret
   81ce0:	aa1403e0 	mov	x0, x20
   81ce4:	97fffdd1 	bl	81428 <new_pd>
   81ce8:	f94166d6 	ldr	x22, [x22, #712]
   81cec:	f8747ac1 	ldr	x1, [x22, x20, lsl #3]
   81cf0:	f9401020 	ldr	x0, [x1, #32]
   81cf4:	17ffffe3 	b	81c80 <kmalloc+0x78>
   81cf8:	aa1503e0 	mov	x0, x21
   81cfc:	52800002 	mov	w2, #0x0                   	// #0
   81d00:	d2800041 	mov	x1, #0x2                   	// #2
   81d04:	940005fb 	bl	834f0 <log_hex>
   81d08:	f9400ae0 	ldr	x0, [x23, #16]
   81d0c:	90000038 	adrp	x24, 85000 <el1_evt+0x1000>
   81d10:	b40005e0 	cbz	x0, 81dcc <kmalloc+0x1c4>
   81d14:	f9400c03 	ldr	x3, [x0, #24]
   81d18:	b5000463 	cbnz	x3, 81da4 <kmalloc+0x19c>
   81d1c:	f9401001 	ldr	x1, [x0, #32]
   81d20:	f100bc3f 	cmp	x1, #0x2f
   81d24:	54000349 	b.ls	81d8c <kmalloc+0x184>  // b.plast
   81d28:	d100c022 	sub	x2, x1, #0x30
   81d2c:	f9001002 	str	x2, [x0, #32]
   81d30:	f9400801 	ldr	x1, [x0, #16]
   81d34:	9100c022 	add	x2, x1, #0x30
   81d38:	f9000802 	str	x2, [x0, #16]
   81d3c:	f94166d5 	ldr	x21, [x22, #712]
   81d40:	aa1403e0 	mov	x0, x20
   81d44:	f8347aa1 	str	x1, [x21, x20, lsl #3]
   81d48:	f900003f 	str	xzr, [x1]
   81d4c:	f9000c3f 	str	xzr, [x1, #24]
   81d50:	f900143f 	str	xzr, [x1, #40]
   81d54:	97fffdb5 	bl	81428 <new_pd>
   81d58:	f9416b18 	ldr	x24, [x24, #720]
   81d5c:	f8747aa1 	ldr	x1, [x21, x20, lsl #3]
   81d60:	f9400300 	ldr	x0, [x24]
   81d64:	f9000420 	str	x0, [x1, #8]
   81d68:	17ffffc1 	b	81c6c <kmalloc+0x64>
   81d6c:	f9400022 	ldr	x2, [x1]
   81d70:	f9400440 	ldr	x0, [x2, #8]
   81d74:	b5000080 	cbnz	x0, 81d84 <kmalloc+0x17c>
   81d78:	f9400042 	ldr	x2, [x2]
   81d7c:	f9400440 	ldr	x0, [x2, #8]
   81d80:	b4ffffc0 	cbz	x0, 81d78 <kmalloc+0x170>
   81d84:	f9000422 	str	x2, [x1, #8]
   81d88:	17ffffcb 	b	81cb4 <kmalloc+0xac>
   81d8c:	d2800040 	mov	x0, #0x2                   	// #2
   81d90:	97fffda6 	bl	81428 <new_pd>
   81d94:	f94166c0 	ldr	x0, [x22, #712]
   81d98:	f9400800 	ldr	x0, [x0, #16]
   81d9c:	f9401001 	ldr	x1, [x0, #32]
   81da0:	17ffffe2 	b	81d28 <kmalloc+0x120>
   81da4:	f9400402 	ldr	x2, [x0, #8]
   81da8:	f9400441 	ldr	x1, [x2, #8]
   81dac:	b4000481 	cbz	x1, 81e3c <kmalloc+0x234>
   81db0:	f9400844 	ldr	x4, [x2, #16]
   81db4:	d1000463 	sub	x3, x3, #0x1
   81db8:	f9400025 	ldr	x5, [x1]
   81dbc:	d1000484 	sub	x4, x4, #0x1
   81dc0:	a9009045 	stp	x5, x4, [x2, #8]
   81dc4:	f9000c03 	str	x3, [x0, #24]
   81dc8:	17ffffdd 	b	81d3c <kmalloc+0x134>
   81dcc:	aa1503e0 	mov	x0, x21
   81dd0:	52800002 	mov	w2, #0x0                   	// #0
   81dd4:	d2800041 	mov	x1, #0x2                   	// #2
   81dd8:	940005c6 	bl	834f0 <log_hex>
   81ddc:	f9400ae0 	ldr	x0, [x23, #16]
   81de0:	b40005e0 	cbz	x0, 81e9c <kmalloc+0x294>
   81de4:	f9400c03 	ldr	x3, [x0, #24]
   81de8:	b50003a3 	cbnz	x3, 81e5c <kmalloc+0x254>
   81dec:	f9401001 	ldr	x1, [x0, #32]
   81df0:	f100bc3f 	cmp	x1, #0x2f
   81df4:	54000489 	b.ls	81e84 <kmalloc+0x27c>  // b.plast
   81df8:	d100c022 	sub	x2, x1, #0x30
   81dfc:	f9001002 	str	x2, [x0, #32]
   81e00:	f9400801 	ldr	x1, [x0, #16]
   81e04:	9100c022 	add	x2, x1, #0x30
   81e08:	f9000802 	str	x2, [x0, #16]
   81e0c:	f94166d5 	ldr	x21, [x22, #712]
   81e10:	d2800040 	mov	x0, #0x2                   	// #2
   81e14:	f9000aa1 	str	x1, [x21, #16]
   81e18:	f900003f 	str	xzr, [x1]
   81e1c:	f9000c3f 	str	xzr, [x1, #24]
   81e20:	f900143f 	str	xzr, [x1, #40]
   81e24:	97fffd81 	bl	81428 <new_pd>
   81e28:	f9416b01 	ldr	x1, [x24, #720]
   81e2c:	f9400aa0 	ldr	x0, [x21, #16]
   81e30:	f9400021 	ldr	x1, [x1]
   81e34:	f9000401 	str	x1, [x0, #8]
   81e38:	17ffffb7 	b	81d14 <kmalloc+0x10c>
   81e3c:	f9400002 	ldr	x2, [x0]
   81e40:	f9400441 	ldr	x1, [x2, #8]
   81e44:	b5000081 	cbnz	x1, 81e54 <kmalloc+0x24c>
   81e48:	f9400042 	ldr	x2, [x2]
   81e4c:	f9400441 	ldr	x1, [x2, #8]
   81e50:	b4ffffc1 	cbz	x1, 81e48 <kmalloc+0x240>
   81e54:	f9000402 	str	x2, [x0, #8]
   81e58:	17ffffd6 	b	81db0 <kmalloc+0x1a8>
   81e5c:	f9400402 	ldr	x2, [x0, #8]
   81e60:	f9400441 	ldr	x1, [x2, #8]
   81e64:	b4000541 	cbz	x1, 81f0c <kmalloc+0x304>
   81e68:	f9400844 	ldr	x4, [x2, #16]
   81e6c:	d1000463 	sub	x3, x3, #0x1
   81e70:	f9400025 	ldr	x5, [x1]
   81e74:	d1000484 	sub	x4, x4, #0x1
   81e78:	a9009045 	stp	x5, x4, [x2, #8]
   81e7c:	f9000c03 	str	x3, [x0, #24]
   81e80:	17ffffe3 	b	81e0c <kmalloc+0x204>
   81e84:	d2800040 	mov	x0, #0x2                   	// #2
   81e88:	97fffd68 	bl	81428 <new_pd>
   81e8c:	f94166c0 	ldr	x0, [x22, #712]
   81e90:	f9400800 	ldr	x0, [x0, #16]
   81e94:	f9401001 	ldr	x1, [x0, #32]
   81e98:	17ffffd8 	b	81df8 <kmalloc+0x1f0>
   81e9c:	aa1503e0 	mov	x0, x21
   81ea0:	52800002 	mov	w2, #0x0                   	// #0
   81ea4:	d2800041 	mov	x1, #0x2                   	// #2
   81ea8:	94000592 	bl	834f0 <log_hex>
   81eac:	f9400ae0 	ldr	x0, [x23, #16]
   81eb0:	b40005e0 	cbz	x0, 81f6c <kmalloc+0x364>
   81eb4:	f9400c02 	ldr	x2, [x0, #24]
   81eb8:	b50003a2 	cbnz	x2, 81f2c <kmalloc+0x324>
   81ebc:	f9401001 	ldr	x1, [x0, #32]
   81ec0:	f100bc3f 	cmp	x1, #0x2f
   81ec4:	54000489 	b.ls	81f54 <kmalloc+0x34c>  // b.plast
   81ec8:	d100c022 	sub	x2, x1, #0x30
   81ecc:	f9001002 	str	x2, [x0, #32]
   81ed0:	f9400801 	ldr	x1, [x0, #16]
   81ed4:	9100c022 	add	x2, x1, #0x30
   81ed8:	f9000802 	str	x2, [x0, #16]
   81edc:	f94166d5 	ldr	x21, [x22, #712]
   81ee0:	d2800040 	mov	x0, #0x2                   	// #2
   81ee4:	f9000aa1 	str	x1, [x21, #16]
   81ee8:	f900003f 	str	xzr, [x1]
   81eec:	f9000c3f 	str	xzr, [x1, #24]
   81ef0:	f900143f 	str	xzr, [x1, #40]
   81ef4:	97fffd4d 	bl	81428 <new_pd>
   81ef8:	f9416b01 	ldr	x1, [x24, #720]
   81efc:	f9400aa0 	ldr	x0, [x21, #16]
   81f00:	f9400021 	ldr	x1, [x1]
   81f04:	f9000401 	str	x1, [x0, #8]
   81f08:	17ffffb7 	b	81de4 <kmalloc+0x1dc>
   81f0c:	f9400002 	ldr	x2, [x0]
   81f10:	f9400441 	ldr	x1, [x2, #8]
   81f14:	b5000081 	cbnz	x1, 81f24 <kmalloc+0x31c>
   81f18:	f9400042 	ldr	x2, [x2]
   81f1c:	f9400441 	ldr	x1, [x2, #8]
   81f20:	b4ffffc1 	cbz	x1, 81f18 <kmalloc+0x310>
   81f24:	f9000402 	str	x2, [x0, #8]
   81f28:	17ffffd0 	b	81e68 <kmalloc+0x260>
   81f2c:	f9400403 	ldr	x3, [x0, #8]
   81f30:	f9400461 	ldr	x1, [x3, #8]
   81f34:	b4000381 	cbz	x1, 81fa4 <kmalloc+0x39c>
   81f38:	a9409061 	ldp	x1, x4, [x3, #8]
   81f3c:	d1000442 	sub	x2, x2, #0x1
   81f40:	f9400025 	ldr	x5, [x1]
   81f44:	d1000484 	sub	x4, x4, #0x1
   81f48:	a9009065 	stp	x5, x4, [x3, #8]
   81f4c:	f9000c02 	str	x2, [x0, #24]
   81f50:	17ffffe3 	b	81edc <kmalloc+0x2d4>
   81f54:	d2800040 	mov	x0, #0x2                   	// #2
   81f58:	97fffd34 	bl	81428 <new_pd>
   81f5c:	f94166c0 	ldr	x0, [x22, #712]
   81f60:	f9400800 	ldr	x0, [x0, #16]
   81f64:	f9401001 	ldr	x1, [x0, #32]
   81f68:	17ffffd8 	b	81ec8 <kmalloc+0x2c0>
   81f6c:	d2800600 	mov	x0, #0x30                  	// #48
   81f70:	97fffd92 	bl	815b8 <alloc_slab>
   81f74:	aa0003e1 	mov	x1, x0
   81f78:	f9000ae1 	str	x1, [x23, #16]
   81f7c:	d2800040 	mov	x0, #0x2                   	// #2
   81f80:	f900003f 	str	xzr, [x1]
   81f84:	f9000c3f 	str	xzr, [x1, #24]
   81f88:	f900143f 	str	xzr, [x1, #40]
   81f8c:	97fffd27 	bl	81428 <new_pd>
   81f90:	f9416b01 	ldr	x1, [x24, #720]
   81f94:	f9400ae0 	ldr	x0, [x23, #16]
   81f98:	f9400021 	ldr	x1, [x1]
   81f9c:	f9000401 	str	x1, [x0, #8]
   81fa0:	17ffffc5 	b	81eb4 <kmalloc+0x2ac>
   81fa4:	f9400001 	ldr	x1, [x0]
   81fa8:	f9400423 	ldr	x3, [x1, #8]
   81fac:	b5000083 	cbnz	x3, 81fbc <kmalloc+0x3b4>
   81fb0:	f9400021 	ldr	x1, [x1]
   81fb4:	f9400423 	ldr	x3, [x1, #8]
   81fb8:	b4ffffc3 	cbz	x3, 81fb0 <kmalloc+0x3a8>
   81fbc:	aa0103e3 	mov	x3, x1
   81fc0:	f9000401 	str	x1, [x0, #8]
   81fc4:	17ffffdd 	b	81f38 <kmalloc+0x330>

0000000000081fc8 <kfree>:
   81fc8:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   81fcc:	910003fd 	mov	x29, sp
   81fd0:	a90153f3 	stp	x19, x20, [sp, #16]
   81fd4:	90000034 	adrp	x20, 85000 <el1_evt+0x1000>
   81fd8:	f9417694 	ldr	x20, [x20, #744]
   81fdc:	f9408281 	ldr	x1, [x20, #256]
   81fe0:	f90013f5 	str	x21, [sp, #32]
   81fe4:	d34cfc15 	lsr	x21, x0, #12
   81fe8:	38756822 	ldrb	w2, [x1, x21]
   81fec:	121b0842 	and	w2, w2, #0xe0
   81ff0:	7100805f 	cmp	w2, #0x20
   81ff4:	54000120 	b.eq	82018 <kfree+0x50>  // b.none
   81ff8:	7101805f 	cmp	w2, #0x60
   81ffc:	54000160 	b.eq	82028 <kfree+0x60>  // b.none
   82000:	7101005f 	cmp	w2, #0x40
   82004:	540001a0 	b.eq	82038 <kfree+0x70>  // b.none
   82008:	a94153f3 	ldp	x19, x20, [sp, #16]
   8200c:	f94013f5 	ldr	x21, [sp, #32]
   82010:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82014:	d65f03c0 	ret
   82018:	a94153f3 	ldp	x19, x20, [sp, #16]
   8201c:	f94013f5 	ldr	x21, [sp, #32]
   82020:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82024:	17fffdaf 	b	816e0 <free_page>
   82028:	a94153f3 	ldp	x19, x20, [sp, #16]
   8202c:	f94013f5 	ldr	x21, [sp, #32]
   82030:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82034:	17fffdfd 	b	81828 <free_reserve>
   82038:	aa0003f3 	mov	x19, x0
   8203c:	52800002 	mov	w2, #0x0                   	// #0
   82040:	aa1303e1 	mov	x1, x19
   82044:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82048:	9103a000 	add	x0, x0, #0xe8
   8204c:	94000529 	bl	834f0 <log_hex>
   82050:	f9408280 	ldr	x0, [x20, #256]
   82054:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   82058:	f9416421 	ldr	x1, [x1, #712]
   8205c:	38756800 	ldrb	w0, [x0, x21]
   82060:	12001000 	and	w0, w0, #0x1f
   82064:	9ac026b5 	lsr	x21, x21, x0
   82068:	9ac022b5 	lsl	x21, x21, x0
   8206c:	d374ceb5 	lsl	x21, x21, #12
   82070:	f9400ea0 	ldr	x0, [x21, #24]
   82074:	f94006a2 	ldr	x2, [x21, #8]
   82078:	f8607820 	ldr	x0, [x1, x0, lsl #3]
   8207c:	f9400403 	ldr	x3, [x0, #8]
   82080:	f9400c01 	ldr	x1, [x0, #24]
   82084:	f9000262 	str	x2, [x19]
   82088:	f9400aa2 	ldr	x2, [x21, #16]
   8208c:	91000421 	add	x1, x1, #0x1
   82090:	91000442 	add	x2, x2, #0x1
   82094:	a9008ab3 	stp	x19, x2, [x21, #8]
   82098:	f9400462 	ldr	x2, [x3, #8]
   8209c:	f9000c01 	str	x1, [x0, #24]
   820a0:	b5fffb42 	cbnz	x2, 82008 <kfree+0x40>
   820a4:	f9000415 	str	x21, [x0, #8]
   820a8:	17ffffd8 	b	82008 <kfree+0x40>
   820ac:	d503201f 	nop

00000000000820b0 <init_kmalloc>:
   820b0:	a9ab7bfd 	stp	x29, x30, [sp, #-336]!
   820b4:	910003fd 	mov	x29, sp
   820b8:	a90153f3 	stp	x19, x20, [sp, #16]
   820bc:	f0000013 	adrp	x19, 85000 <el1_evt+0x1000>
   820c0:	f9415a60 	ldr	x0, [x19, #688]
   820c4:	a9025bf5 	stp	x21, x22, [sp, #32]
   820c8:	a90363f7 	stp	x23, x24, [sp, #48]
   820cc:	d34cfc00 	lsr	x0, x0, #12
   820d0:	97fffb44 	bl	80de0 <reserve_alloc>
   820d4:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   820d8:	f0000002 	adrp	x2, 85000 <el1_evt+0x1000>
   820dc:	f9417423 	ldr	x3, [x1, #744]
   820e0:	aa0003e1 	mov	x1, x0
   820e4:	9103e040 	add	x0, x2, #0xf8
   820e8:	52800002 	mov	w2, #0x0                   	// #0
   820ec:	f9008061 	str	x1, [x3, #256]
   820f0:	94000500 	bl	834f0 <log_hex>
   820f4:	90000022 	adrp	x2, 86000 <sa>
   820f8:	91000040 	add	x0, x2, #0x0
   820fc:	f9408001 	ldr	x1, [x0, #256]
   82100:	f90093e1 	str	x1, [sp, #288]
   82104:	71000421 	subs	w1, w1, #0x1
   82108:	54002944 	b.mi	82630 <init_kmalloc+0x580>  // b.first
   8210c:	9104c3fe 	add	x30, sp, #0x130
   82110:	910513e3 	add	x3, sp, #0x144
   82114:	a9046bf9 	stp	x25, x26, [sp, #64]
   82118:	910423fa 	add	x26, sp, #0x108
   8211c:	52800019 	mov	w25, #0x0                   	// #0
   82120:	a90573fb 	stp	x27, x28, [sp, #80]
   82124:	910463fc 	add	x28, sp, #0x118
   82128:	5280001b 	mov	w27, #0x0                   	// #0
   8212c:	f940004a 	ldr	x10, [x2]
   82130:	293efc7f 	stp	wzr, wzr, [x3, #-12]
   82134:	293f7f5f 	stp	wzr, wzr, [x26, #-8]
   82138:	52800015 	mov	w21, #0x0                   	// #0
   8213c:	293f7f9f 	stp	wzr, wzr, [x28, #-8]
   82140:	52800012 	mov	w18, #0x0                   	// #0
   82144:	293f7fdf 	stp	wzr, wzr, [x30, #-8]
   82148:	5280000f 	mov	w15, #0x0                   	// #0
   8214c:	29007f5f 	stp	wzr, wzr, [x26]
   82150:	5280000c 	mov	w12, #0x0                   	// #0
   82154:	29007f9f 	stp	wzr, wzr, [x28]
   82158:	52800008 	mov	w8, #0x0                   	// #0
   8215c:	29007fdf 	stp	wzr, wzr, [x30]
   82160:	52800006 	mov	w6, #0x0                   	// #0
   82164:	2900fc7f 	stp	wzr, wzr, [x3, #4]
   82168:	29117fff 	stp	wzr, wzr, [sp, #136]
   8216c:	29167fff 	stp	wzr, wzr, [sp, #176]
   82170:	291b7fff 	stp	wzr, wzr, [sp, #216]
   82174:	291c7fff 	stp	wzr, wzr, [sp, #224]
   82178:	a9409404 	ldp	x4, x5, [x0, #8]
   8217c:	f9400c07 	ldr	x7, [x0, #24]
   82180:	a9423409 	ldp	x9, x13, [x0, #32]
   82184:	a9450c1c 	ldp	x28, x3, [x0, #80]
   82188:	f9004fe3 	str	x3, [sp, #152]
   8218c:	f9403003 	ldr	x3, [x0, #96]
   82190:	f90057e3 	str	x3, [sp, #168]
   82194:	f9403403 	ldr	x3, [x0, #104]
   82198:	f90067e3 	str	x3, [sp, #200]
   8219c:	f9403803 	ldr	x3, [x0, #112]
   821a0:	f90063e3 	str	x3, [sp, #192]
   821a4:	f9403c03 	ldr	x3, [x0, #120]
   821a8:	f9007be3 	str	x3, [sp, #240]
   821ac:	f9404003 	ldr	x3, [x0, #128]
   821b0:	f90037e3 	str	x3, [sp, #104]
   821b4:	f9405c03 	ldr	x3, [x0, #184]
   821b8:	f9003fe3 	str	x3, [sp, #120]
   821bc:	f9406003 	ldr	x3, [x0, #192]
   821c0:	f90043e3 	str	x3, [sp, #128]
   821c4:	f9406403 	ldr	x3, [x0, #200]
   821c8:	f9004be3 	str	x3, [sp, #144]
   821cc:	f9406803 	ldr	x3, [x0, #208]
   821d0:	a9437810 	ldp	x16, x30, [x0, #48]
   821d4:	a9446016 	ldp	x22, x24, [x0, #64]
   821d8:	a948b80b 	ldp	x11, x14, [x0, #136]
   821dc:	a949d011 	ldp	x17, x20, [x0, #152]
   821e0:	a94ae817 	ldp	x23, x26, [x0, #168]
   821e4:	f90053e3 	str	x3, [sp, #160]
   821e8:	f9406c03 	ldr	x3, [x0, #216]
   821ec:	f9005fe3 	str	x3, [sp, #184]
   821f0:	f9407003 	ldr	x3, [x0, #224]
   821f4:	f9006be3 	str	x3, [sp, #208]
   821f8:	f9407403 	ldr	x3, [x0, #232]
   821fc:	f90077e3 	str	x3, [sp, #232]
   82200:	a94f0003 	ldp	x3, x0, [x0, #240]
   82204:	f9003bfc 	str	x28, [sp, #112]
   82208:	f9007fe3 	str	x3, [sp, #248]
   8220c:	f900a3e0 	str	x0, [sp, #320]
   82210:	7100003f 	cmp	w1, #0x0
   82214:	54000f8d 	b.le	82404 <init_kmalloc+0x354>
   82218:	eb04015f 	cmp	x10, x4
   8221c:	54003448 	b.hi	828a4 <init_kmalloc+0x7f4>  // b.pmore
   82220:	aa0b03e3 	mov	x3, x11
   82224:	aa0403e0 	mov	x0, x4
   82228:	7100043f 	cmp	w1, #0x1
   8222c:	540034e0 	b.eq	828c8 <init_kmalloc+0x818>  // b.none
   82230:	eb05001f 	cmp	x0, x5
   82234:	54003528 	b.hi	828d8 <init_kmalloc+0x828>  // b.pmore
   82238:	aa0303eb 	mov	x11, x3
   8223c:	aa0003e4 	mov	x4, x0
   82240:	aa0e03e3 	mov	x3, x14
   82244:	aa0503e0 	mov	x0, x5
   82248:	7100083f 	cmp	w1, #0x2
   8224c:	54003540 	b.eq	828f4 <init_kmalloc+0x844>  // b.none
   82250:	eb07001f 	cmp	x0, x7
   82254:	54003588 	b.hi	82904 <init_kmalloc+0x854>  // b.pmore
   82258:	aa0303ee 	mov	x14, x3
   8225c:	aa0003e5 	mov	x5, x0
   82260:	aa1103e3 	mov	x3, x17
   82264:	aa0703e0 	mov	x0, x7
   82268:	71000c3f 	cmp	w1, #0x3
   8226c:	540035c0 	b.eq	82924 <init_kmalloc+0x874>  // b.none
   82270:	eb09001f 	cmp	x0, x9
   82274:	54003608 	b.hi	82934 <init_kmalloc+0x884>  // b.pmore
   82278:	aa0303f1 	mov	x17, x3
   8227c:	aa0003e7 	mov	x7, x0
   82280:	aa1403e3 	mov	x3, x20
   82284:	aa0903e0 	mov	x0, x9
   82288:	7100103f 	cmp	w1, #0x4
   8228c:	54003620 	b.eq	82950 <init_kmalloc+0x8a0>  // b.none
   82290:	eb0d001f 	cmp	x0, x13
   82294:	54003668 	b.hi	82960 <init_kmalloc+0x8b0>  // b.pmore
   82298:	aa0303f4 	mov	x20, x3
   8229c:	aa0003e9 	mov	x9, x0
   822a0:	aa1703e3 	mov	x3, x23
   822a4:	aa0d03e0 	mov	x0, x13
   822a8:	7100143f 	cmp	w1, #0x5
   822ac:	540036a0 	b.eq	82980 <init_kmalloc+0x8d0>  // b.none
   822b0:	eb10001f 	cmp	x0, x16
   822b4:	540036e8 	b.hi	82990 <init_kmalloc+0x8e0>  // b.pmore
   822b8:	aa0303f7 	mov	x23, x3
   822bc:	aa0003ed 	mov	x13, x0
   822c0:	aa1a03e3 	mov	x3, x26
   822c4:	aa1003e0 	mov	x0, x16
   822c8:	7100183f 	cmp	w1, #0x6
   822cc:	54003700 	b.eq	829ac <init_kmalloc+0x8fc>  // b.none
   822d0:	eb1e001f 	cmp	x0, x30
   822d4:	54003748 	b.hi	829bc <init_kmalloc+0x90c>  // b.pmore
   822d8:	aa0303fa 	mov	x26, x3
   822dc:	aa0003f0 	mov	x16, x0
   822e0:	71001c3f 	cmp	w1, #0x7
   822e4:	aa1e03e0 	mov	x0, x30
   822e8:	f9403fe3 	ldr	x3, [sp, #120]
   822ec:	54003780 	b.eq	829dc <init_kmalloc+0x92c>  // b.none
   822f0:	eb16001f 	cmp	x0, x22
   822f4:	540037c8 	b.hi	829ec <init_kmalloc+0x93c>  // b.pmore
   822f8:	f9003fe3 	str	x3, [sp, #120]
   822fc:	aa0003fe 	mov	x30, x0
   82300:	7100203f 	cmp	w1, #0x8
   82304:	aa1603e0 	mov	x0, x22
   82308:	f94043e3 	ldr	x3, [sp, #128]
   8230c:	54003820 	b.eq	82a10 <init_kmalloc+0x960>  // b.none
   82310:	eb18001f 	cmp	x0, x24
   82314:	54003868 	b.hi	82a20 <init_kmalloc+0x970>  // b.pmore
   82318:	f90043e3 	str	x3, [sp, #128]
   8231c:	aa0003f6 	mov	x22, x0
   82320:	7100243f 	cmp	w1, #0x9
   82324:	aa1803e0 	mov	x0, x24
   82328:	f9404be3 	ldr	x3, [sp, #144]
   8232c:	540038e0 	b.eq	82a48 <init_kmalloc+0x998>  // b.none
   82330:	f9403bfc 	ldr	x28, [sp, #112]
   82334:	eb1c001f 	cmp	x0, x28
   82338:	54003908 	b.hi	82a58 <init_kmalloc+0x9a8>  // b.pmore
   8233c:	f9004be3 	str	x3, [sp, #144]
   82340:	aa0003f8 	mov	x24, x0
   82344:	7100283f 	cmp	w1, #0xa
   82348:	aa1c03e0 	mov	x0, x28
   8234c:	f94053e3 	ldr	x3, [sp, #160]
   82350:	54003960 	b.eq	82a7c <init_kmalloc+0x9cc>  // b.none
   82354:	f9404ffc 	ldr	x28, [sp, #152]
   82358:	eb1c001f 	cmp	x0, x28
   8235c:	54003988 	b.hi	82a8c <init_kmalloc+0x9dc>  // b.pmore
   82360:	f9003be0 	str	x0, [sp, #112]
   82364:	71002c3f 	cmp	w1, #0xb
   82368:	f90053e3 	str	x3, [sp, #160]
   8236c:	aa1c03e0 	mov	x0, x28
   82370:	f9405fe3 	ldr	x3, [sp, #184]
   82374:	54003a20 	b.eq	82ab8 <init_kmalloc+0xa08>  // b.none
   82378:	f94057fc 	ldr	x28, [sp, #168]
   8237c:	eb1c001f 	cmp	x0, x28
   82380:	54003a48 	b.hi	82ac8 <init_kmalloc+0xa18>  // b.pmore
   82384:	f9004fe0 	str	x0, [sp, #152]
   82388:	7100303f 	cmp	w1, #0xc
   8238c:	f9005fe3 	str	x3, [sp, #184]
   82390:	aa1c03e0 	mov	x0, x28
   82394:	f9406be3 	ldr	x3, [sp, #208]
   82398:	54003ae0 	b.eq	82af4 <init_kmalloc+0xa44>  // b.none
   8239c:	f94067fc 	ldr	x28, [sp, #200]
   823a0:	eb1c001f 	cmp	x0, x28
   823a4:	54003d08 	b.hi	82b44 <init_kmalloc+0xa94>  // b.pmore
   823a8:	f90057e0 	str	x0, [sp, #168]
   823ac:	7100343f 	cmp	w1, #0xd
   823b0:	f9006be3 	str	x3, [sp, #208]
   823b4:	aa1c03e0 	mov	x0, x28
   823b8:	f94077e3 	ldr	x3, [sp, #232]
   823bc:	54003da0 	b.eq	82b70 <init_kmalloc+0xac0>  // b.none
   823c0:	f94063fc 	ldr	x28, [sp, #192]
   823c4:	eb1c001f 	cmp	x0, x28
   823c8:	54003dc8 	b.hi	82b80 <init_kmalloc+0xad0>  // b.pmore
   823cc:	f90067e0 	str	x0, [sp, #200]
   823d0:	7100383f 	cmp	w1, #0xe
   823d4:	f90077e3 	str	x3, [sp, #232]
   823d8:	aa1c03e0 	mov	x0, x28
   823dc:	f9407fe3 	ldr	x3, [sp, #248]
   823e0:	54003e60 	b.eq	82bac <init_kmalloc+0xafc>  // b.none
   823e4:	f9407bfc 	ldr	x28, [sp, #240]
   823e8:	eb1c001f 	cmp	x0, x28
   823ec:	54003e88 	b.hi	82bbc <init_kmalloc+0xb0c>  // b.pmore
   823f0:	f90063e0 	str	x0, [sp, #192]
   823f4:	51000421 	sub	w1, w1, #0x1
   823f8:	f9007fe3 	str	x3, [sp, #248]
   823fc:	7100003f 	cmp	w1, #0x0
   82400:	54fff0cc 	b.gt	82218 <init_kmalloc+0x168>
   82404:	71000421 	subs	w1, w1, #0x1
   82408:	54fff045 	b.pl	82210 <init_kmalloc+0x160>  // b.nfrst
   8240c:	b9411be0 	ldr	w0, [sp, #280]
   82410:	f9403bfc 	ldr	x28, [sp, #112]
   82414:	34000080 	cbz	w0, 82424 <init_kmalloc+0x374>
   82418:	91000040 	add	x0, x2, #0x0
   8241c:	f94063e1 	ldr	x1, [sp, #192]
   82420:	f9003801 	str	x1, [x0, #112]
   82424:	b9414be0 	ldr	w0, [sp, #328]
   82428:	34000080 	cbz	w0, 82438 <init_kmalloc+0x388>
   8242c:	91000040 	add	x0, x2, #0x0
   82430:	f9407be1 	ldr	x1, [sp, #240]
   82434:	f9003c01 	str	x1, [x0, #120]
   82438:	b94137e0 	ldr	w0, [sp, #308]
   8243c:	34000080 	cbz	w0, 8244c <init_kmalloc+0x39c>
   82440:	91000040 	add	x0, x2, #0x0
   82444:	f9407fe1 	ldr	x1, [sp, #248]
   82448:	f9007801 	str	x1, [x0, #240]
   8244c:	b9414fe0 	ldr	w0, [sp, #332]
   82450:	34000080 	cbz	w0, 82460 <init_kmalloc+0x3b0>
   82454:	91000040 	add	x0, x2, #0x0
   82458:	f940a3e1 	ldr	x1, [sp, #320]
   8245c:	f9007c01 	str	x1, [x0, #248]
   82460:	b940e3e0 	ldr	w0, [sp, #224]
   82464:	34000040 	cbz	w0, 8246c <init_kmalloc+0x3bc>
   82468:	f900004a 	str	x10, [x2]
   8246c:	b9408be0 	ldr	w0, [sp, #136]
   82470:	34000060 	cbz	w0, 8247c <init_kmalloc+0x3cc>
   82474:	91000040 	add	x0, x2, #0x0
   82478:	f9000404 	str	x4, [x0, #8]
   8247c:	b940e7e0 	ldr	w0, [sp, #228]
   82480:	34000080 	cbz	w0, 82490 <init_kmalloc+0x3e0>
   82484:	91000040 	add	x0, x2, #0x0
   82488:	f94037e1 	ldr	x1, [sp, #104]
   8248c:	f9004001 	str	x1, [x0, #128]
   82490:	34000066 	cbz	w6, 8249c <init_kmalloc+0x3ec>
   82494:	91000040 	add	x0, x2, #0x0
   82498:	f900440b 	str	x11, [x0, #136]
   8249c:	b9408fe0 	ldr	w0, [sp, #140]
   824a0:	34000060 	cbz	w0, 824ac <init_kmalloc+0x3fc>
   824a4:	91000040 	add	x0, x2, #0x0
   824a8:	f9000805 	str	x5, [x0, #16]
   824ac:	34000068 	cbz	w8, 824b8 <init_kmalloc+0x408>
   824b0:	91000040 	add	x0, x2, #0x0
   824b4:	f900480e 	str	x14, [x0, #144]
   824b8:	b940b3e0 	ldr	w0, [sp, #176]
   824bc:	34000060 	cbz	w0, 824c8 <init_kmalloc+0x418>
   824c0:	91000040 	add	x0, x2, #0x0
   824c4:	f9000c07 	str	x7, [x0, #24]
   824c8:	3400006c 	cbz	w12, 824d4 <init_kmalloc+0x424>
   824cc:	91000040 	add	x0, x2, #0x0
   824d0:	f9004c11 	str	x17, [x0, #152]
   824d4:	b940b7e0 	ldr	w0, [sp, #180]
   824d8:	34000060 	cbz	w0, 824e4 <init_kmalloc+0x434>
   824dc:	91000040 	add	x0, x2, #0x0
   824e0:	f9001009 	str	x9, [x0, #32]
   824e4:	3400006f 	cbz	w15, 824f0 <init_kmalloc+0x440>
   824e8:	91000040 	add	x0, x2, #0x0
   824ec:	f9005014 	str	x20, [x0, #160]
   824f0:	b940dbe0 	ldr	w0, [sp, #216]
   824f4:	34000060 	cbz	w0, 82500 <init_kmalloc+0x450>
   824f8:	91000040 	add	x0, x2, #0x0
   824fc:	f900140d 	str	x13, [x0, #40]
   82500:	34000072 	cbz	w18, 8250c <init_kmalloc+0x45c>
   82504:	91000040 	add	x0, x2, #0x0
   82508:	f9005417 	str	x23, [x0, #168]
   8250c:	b940dfe0 	ldr	w0, [sp, #220]
   82510:	34000060 	cbz	w0, 8251c <init_kmalloc+0x46c>
   82514:	91000040 	add	x0, x2, #0x0
   82518:	f9001810 	str	x16, [x0, #48]
   8251c:	34000075 	cbz	w21, 82528 <init_kmalloc+0x478>
   82520:	91000040 	add	x0, x2, #0x0
   82524:	f900581a 	str	x26, [x0, #176]
   82528:	b94103e0 	ldr	w0, [sp, #256]
   8252c:	34000060 	cbz	w0, 82538 <init_kmalloc+0x488>
   82530:	91000040 	add	x0, x2, #0x0
   82534:	f9001c1e 	str	x30, [x0, #56]
   82538:	34000099 	cbz	w25, 82548 <init_kmalloc+0x498>
   8253c:	91000040 	add	x0, x2, #0x0
   82540:	f9403fe1 	ldr	x1, [sp, #120]
   82544:	f9005c01 	str	x1, [x0, #184]
   82548:	b94107e0 	ldr	w0, [sp, #260]
   8254c:	34000060 	cbz	w0, 82558 <init_kmalloc+0x4a8>
   82550:	91000040 	add	x0, x2, #0x0
   82554:	f9002016 	str	x22, [x0, #64]
   82558:	3400009b 	cbz	w27, 82568 <init_kmalloc+0x4b8>
   8255c:	91000040 	add	x0, x2, #0x0
   82560:	f94043e1 	ldr	x1, [sp, #128]
   82564:	f9006001 	str	x1, [x0, #192]
   82568:	b9410be0 	ldr	w0, [sp, #264]
   8256c:	34000060 	cbz	w0, 82578 <init_kmalloc+0x4c8>
   82570:	91000040 	add	x0, x2, #0x0
   82574:	f9002418 	str	x24, [x0, #72]
   82578:	b9410fe0 	ldr	w0, [sp, #268]
   8257c:	34000080 	cbz	w0, 8258c <init_kmalloc+0x4dc>
   82580:	91000040 	add	x0, x2, #0x0
   82584:	f9404be1 	ldr	x1, [sp, #144]
   82588:	f9006401 	str	x1, [x0, #200]
   8258c:	b94113e0 	ldr	w0, [sp, #272]
   82590:	34000060 	cbz	w0, 8259c <init_kmalloc+0x4ec>
   82594:	91000040 	add	x0, x2, #0x0
   82598:	f900281c 	str	x28, [x0, #80]
   8259c:	b94117e0 	ldr	w0, [sp, #276]
   825a0:	34000080 	cbz	w0, 825b0 <init_kmalloc+0x500>
   825a4:	91000040 	add	x0, x2, #0x0
   825a8:	f94053e1 	ldr	x1, [sp, #160]
   825ac:	f9006801 	str	x1, [x0, #208]
   825b0:	b9411fe0 	ldr	w0, [sp, #284]
   825b4:	34000080 	cbz	w0, 825c4 <init_kmalloc+0x514>
   825b8:	91000040 	add	x0, x2, #0x0
   825bc:	f9404fe1 	ldr	x1, [sp, #152]
   825c0:	f9002c01 	str	x1, [x0, #88]
   825c4:	b9412be0 	ldr	w0, [sp, #296]
   825c8:	34000080 	cbz	w0, 825d8 <init_kmalloc+0x528>
   825cc:	91000040 	add	x0, x2, #0x0
   825d0:	f9405fe1 	ldr	x1, [sp, #184]
   825d4:	f9006c01 	str	x1, [x0, #216]
   825d8:	b9412fe0 	ldr	w0, [sp, #300]
   825dc:	34000080 	cbz	w0, 825ec <init_kmalloc+0x53c>
   825e0:	91000040 	add	x0, x2, #0x0
   825e4:	f94057e1 	ldr	x1, [sp, #168]
   825e8:	f9003001 	str	x1, [x0, #96]
   825ec:	b94133e0 	ldr	w0, [sp, #304]
   825f0:	34000080 	cbz	w0, 82600 <init_kmalloc+0x550>
   825f4:	91000040 	add	x0, x2, #0x0
   825f8:	f9406be1 	ldr	x1, [sp, #208]
   825fc:	f9007001 	str	x1, [x0, #224]
   82600:	b9413be0 	ldr	w0, [sp, #312]
   82604:	34000080 	cbz	w0, 82614 <init_kmalloc+0x564>
   82608:	91000040 	add	x0, x2, #0x0
   8260c:	f94067e1 	ldr	x1, [sp, #200]
   82610:	f9003401 	str	x1, [x0, #104]
   82614:	b9413fe0 	ldr	w0, [sp, #316]
   82618:	34002f00 	cbz	w0, 82bf8 <init_kmalloc+0xb48>
   8261c:	91000040 	add	x0, x2, #0x0
   82620:	f94077e1 	ldr	x1, [sp, #232]
   82624:	a9446bf9 	ldp	x25, x26, [sp, #64]
   82628:	a94573fb 	ldp	x27, x28, [sp, #80]
   8262c:	f9007401 	str	x1, [x0, #232]
   82630:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82634:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   82638:	f0000003 	adrp	x3, 85000 <el1_evt+0x1000>
   8263c:	f0000009 	adrp	x9, 85000 <el1_evt+0x1000>
   82640:	f9417400 	ldr	x0, [x0, #744]
   82644:	3dc04430 	ldr	q16, [x1, #272]
   82648:	4e080c04 	dup	v4.2d, x0
   8264c:	3dc04867 	ldr	q7, [x3, #288]
   82650:	f0000003 	adrp	x3, 85000 <el1_evt+0x1000>
   82654:	aa0003e1 	mov	x1, x0
   82658:	91018007 	add	x7, x0, #0x60
   8265c:	4ef08490 	add	v16.2d, v4.2d, v16.2d
   82660:	91010008 	add	x8, x0, #0x40
   82664:	4ee78487 	add	v7.2d, v4.2d, v7.2d
   82668:	91020006 	add	x6, x0, #0x80
   8266c:	3dc04c66 	ldr	q6, [x3, #304]
   82670:	91028005 	add	x5, x0, #0xa0
   82674:	4eb01e02 	mov	v2.16b, v16.16b
   82678:	91030004 	add	x4, x0, #0xc0
   8267c:	4eb01e03 	mov	v3.16b, v16.16b
   82680:	91038003 	add	x3, x0, #0xe0
   82684:	4ea71ce0 	mov	v0.16b, v7.16b
   82688:	4ea71ce1 	mov	v1.16b, v7.16b
   8268c:	4ee68486 	add	v6.2d, v4.2d, v6.2d
   82690:	4c9f8c22 	st2	{v2.2d, v3.2d}, [x1], #32
   82694:	3dc05125 	ldr	q5, [x9, #320]
   82698:	4c008c20 	st2	{v0.2d, v1.2d}, [x1]
   8269c:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   826a0:	4ea61cc2 	mov	v2.16b, v6.16b
   826a4:	3dc05427 	ldr	q7, [x1, #336]
   826a8:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   826ac:	4ee58485 	add	v5.2d, v4.2d, v5.2d
   826b0:	4ea61cc3 	mov	v3.16b, v6.16b
   826b4:	3dc05826 	ldr	q6, [x1, #352]
   826b8:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   826bc:	4ea51ca0 	mov	v0.16b, v5.16b
   826c0:	4ea51ca1 	mov	v1.16b, v5.16b
   826c4:	4c008d02 	st2	{v2.2d, v3.2d}, [x8]
   826c8:	4ee68486 	add	v6.2d, v4.2d, v6.2d
   826cc:	3dc05c25 	ldr	q5, [x1, #368]
   826d0:	f0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   826d4:	4ee78487 	add	v7.2d, v4.2d, v7.2d
   826d8:	4c008ce0 	st2	{v0.2d, v1.2d}, [x7]
   826dc:	4ea61cc0 	mov	v0.16b, v6.16b
   826e0:	4ea61cc1 	mov	v1.16b, v6.16b
   826e4:	3dc06026 	ldr	q6, [x1, #384]
   826e8:	4ee58485 	add	v5.2d, v4.2d, v5.2d
   826ec:	4ea71ce2 	mov	v2.16b, v7.16b
   826f0:	4c008ca0 	st2	{v0.2d, v1.2d}, [x5]
   826f4:	4ea71ce3 	mov	v3.16b, v7.16b
   826f8:	4ee68484 	add	v4.2d, v4.2d, v6.2d
   826fc:	f9415a61 	ldr	x1, [x19, #688]
   82700:	4c008cc2 	st2	{v2.2d, v3.2d}, [x6]
   82704:	4ea41c80 	mov	v0.16b, v4.16b
   82708:	eb4133ff 	cmp	xzr, x1, lsr #12
   8270c:	4ea51ca2 	mov	v2.16b, v5.16b
   82710:	4ea51ca3 	mov	v3.16b, v5.16b
   82714:	4ea41c81 	mov	v1.16b, v4.16b
   82718:	4c008c82 	st2	{v2.2d, v3.2d}, [x4]
   8271c:	4c008c60 	st2	{v0.2d, v1.2d}, [x3]
   82720:	540001a0 	b.eq	82754 <init_kmalloc+0x6a4>  // b.none
   82724:	aa0003e3 	mov	x3, x0
   82728:	52800404 	mov	w4, #0x20                  	// #32
   8272c:	d2800000 	mov	x0, #0x0                   	// #0
   82730:	f9408061 	ldr	x1, [x3, #256]
   82734:	38206824 	strb	w4, [x1, x0]
   82738:	91000400 	add	x0, x0, #0x1
   8273c:	f9415a61 	ldr	x1, [x19, #688]
   82740:	eb41301f 	cmp	x0, x1, lsr #12
   82744:	54ffff63 	b.cc	82730 <init_kmalloc+0x680>  // b.lo, b.ul, b.last
   82748:	91000040 	add	x0, x2, #0x0
   8274c:	f9408000 	ldr	x0, [x0, #256]
   82750:	f90093e0 	str	x0, [sp, #288]
   82754:	f94093e0 	ldr	x0, [sp, #288]
   82758:	b40024c0 	cbz	x0, 82bf0 <init_kmalloc+0xb40>
   8275c:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82760:	91000056 	add	x22, x2, #0x0
   82764:	aa1603f8 	mov	x24, x22
   82768:	d2800017 	mov	x23, #0x0                   	// #0
   8276c:	f9417415 	ldr	x21, [x0, #744]
   82770:	528001f4 	mov	w20, #0xf                   	// #15
   82774:	d2800000 	mov	x0, #0x0                   	// #0
   82778:	52800032 	mov	w18, #0x1                   	// #1
   8277c:	d503201f 	nop
   82780:	f94002c1 	ldr	x1, [x22]
   82784:	97fff6e7 	bl	80320 <place_buddy.constprop.1>
   82788:	f94002c3 	ldr	x3, [x22]
   8278c:	f94042c0 	ldr	x0, [x22, #128]
   82790:	8b000060 	add	x0, x3, x0
   82794:	eb00007f 	cmp	x3, x0
   82798:	540004a0 	b.eq	8282c <init_kmalloc+0x77c>  // b.none
   8279c:	dac00069 	rbit	x9, x3
   827a0:	dac00008 	rbit	x8, x0
   827a4:	dac01129 	clz	x9, x9
   827a8:	dac01108 	clz	x8, x8
   827ac:	d503201f 	nop
   827b0:	51003122 	sub	w2, w9, #0xc
   827b4:	51003101 	sub	w1, w8, #0xc
   827b8:	71003c5f 	cmp	w2, #0xf
   827bc:	d34cfc67 	lsr	x7, x3, #12
   827c0:	1a94d042 	csel	w2, w2, w20, le
   827c4:	71003c3f 	cmp	w1, #0xf
   827c8:	1a94d021 	csel	w1, w1, w20, le
   827cc:	11003045 	add	w5, w2, #0xc
   827d0:	11003024 	add	w4, w1, #0xc
   827d4:	6b01005f 	cmp	w2, w1
   827d8:	1ac52245 	lsl	w5, w18, w5
   827dc:	1ac42244 	lsl	w4, w18, w4
   827e0:	f94082a6 	ldr	x6, [x21, #256]
   827e4:	5400190c 	b.gt	82b04 <init_kmalloc+0xa54>
   827e8:	386768c1 	ldrb	w1, [x6, x7]
   827ec:	8b25c063 	add	x3, x3, w5, sxtw
   827f0:	dac00069 	rbit	x9, x3
   827f4:	eb00007f 	cmp	x3, x0
   827f8:	12001021 	and	w1, w1, #0x1f
   827fc:	dac01129 	clz	x9, x9
   82800:	321b0421 	orr	w1, w1, #0x60
   82804:	382768c1 	strb	w1, [x6, x7]
   82808:	f94082a4 	ldr	x4, [x21, #256]
   8280c:	38676881 	ldrb	w1, [x4, x7]
   82810:	121b6821 	and	w1, w1, #0xffffffe0
   82814:	2a020022 	orr	w2, w1, w2
   82818:	38276882 	strb	w2, [x4, x7]
   8281c:	54fffca1 	b.ne	827b0 <init_kmalloc+0x700>  // b.any
   82820:	f94002c1 	ldr	x1, [x22]
   82824:	f94042c0 	ldr	x0, [x22, #128]
   82828:	8b000020 	add	x0, x1, x0
   8282c:	f9408301 	ldr	x1, [x24, #256]
   82830:	910006f7 	add	x23, x23, #0x1
   82834:	910022d6 	add	x22, x22, #0x8
   82838:	eb17003f 	cmp	x1, x23
   8283c:	54fffa28 	b.hi	82780 <init_kmalloc+0x6d0>  // b.pmore
   82840:	f9415a61 	ldr	x1, [x19, #688]
   82844:	97fff6b7 	bl	80320 <place_buddy.constprop.1>
   82848:	4f000400 	movi	v0.4s, #0x0
   8284c:	f0000002 	adrp	x2, 85000 <el1_evt+0x1000>
   82850:	f9416440 	ldr	x0, [x2, #712]
   82854:	91100001 	add	x1, x0, #0x400
   82858:	3c810400 	str	q0, [x0], #16
   8285c:	eb01001f 	cmp	x0, x1
   82860:	54ffffc1 	b.ne	82858 <init_kmalloc+0x7a8>  // b.any
   82864:	f0000013 	adrp	x19, 85000 <el1_evt+0x1000>
   82868:	d2800040 	mov	x0, #0x2                   	// #2
   8286c:	f9416442 	ldr	x2, [x2, #712]
   82870:	f9416a73 	ldr	x19, [x19, #720]
   82874:	f9000853 	str	x19, [x2, #16]
   82878:	f900027f 	str	xzr, [x19]
   8287c:	f9000e7f 	str	xzr, [x19, #24]
   82880:	f900167f 	str	xzr, [x19, #40]
   82884:	97fffae9 	bl	81428 <new_pd>
   82888:	f9400260 	ldr	x0, [x19]
   8288c:	a9425bf5 	ldp	x21, x22, [sp, #32]
   82890:	a94363f7 	ldp	x23, x24, [sp, #48]
   82894:	f9000660 	str	x0, [x19, #8]
   82898:	a94153f3 	ldp	x19, x20, [sp, #16]
   8289c:	a8d57bfd 	ldp	x29, x30, [sp], #336
   828a0:	d65f03c0 	ret
   828a4:	52800026 	mov	w6, #0x1                   	// #1
   828a8:	b9008be6 	str	w6, [sp, #136]
   828ac:	291c1be6 	stp	w6, w6, [sp, #224]
   828b0:	aa0a03e0 	mov	x0, x10
   828b4:	f94037e3 	ldr	x3, [sp, #104]
   828b8:	f90037eb 	str	x11, [sp, #104]
   828bc:	aa0403ea 	mov	x10, x4
   828c0:	7100043f 	cmp	w1, #0x1
   828c4:	54ffcb61 	b.ne	82230 <init_kmalloc+0x180>  // b.any
   828c8:	aa0303eb 	mov	x11, x3
   828cc:	aa0003e4 	mov	x4, x0
   828d0:	52800001 	mov	w1, #0x0                   	// #0
   828d4:	17fffe4f 	b	82210 <init_kmalloc+0x160>
   828d8:	52800028 	mov	w8, #0x1                   	// #1
   828dc:	291123e8 	stp	w8, w8, [sp, #136]
   828e0:	aa0e03eb 	mov	x11, x14
   828e4:	aa0503e4 	mov	x4, x5
   828e8:	2a0803e6 	mov	w6, w8
   828ec:	7100083f 	cmp	w1, #0x2
   828f0:	54ffcb01 	b.ne	82250 <init_kmalloc+0x1a0>  // b.any
   828f4:	aa0303ee 	mov	x14, x3
   828f8:	aa0003e5 	mov	x5, x0
   828fc:	52800021 	mov	w1, #0x1                   	// #1
   82900:	17fffe44 	b	82210 <init_kmalloc+0x160>
   82904:	5280002c 	mov	w12, #0x1                   	// #1
   82908:	b9008fec 	str	w12, [sp, #140]
   8290c:	b900b3ec 	str	w12, [sp, #176]
   82910:	aa1103ee 	mov	x14, x17
   82914:	aa0703e5 	mov	x5, x7
   82918:	2a0c03e8 	mov	w8, w12
   8291c:	71000c3f 	cmp	w1, #0x3
   82920:	54ffca81 	b.ne	82270 <init_kmalloc+0x1c0>  // b.any
   82924:	aa0303f1 	mov	x17, x3
   82928:	aa0003e7 	mov	x7, x0
   8292c:	52800041 	mov	w1, #0x2                   	// #2
   82930:	17fffe38 	b	82210 <init_kmalloc+0x160>
   82934:	5280002f 	mov	w15, #0x1                   	// #1
   82938:	29163fef 	stp	w15, w15, [sp, #176]
   8293c:	aa1403f1 	mov	x17, x20
   82940:	aa0903e7 	mov	x7, x9
   82944:	2a0f03ec 	mov	w12, w15
   82948:	7100103f 	cmp	w1, #0x4
   8294c:	54ffca21 	b.ne	82290 <init_kmalloc+0x1e0>  // b.any
   82950:	aa0303f4 	mov	x20, x3
   82954:	aa0003e9 	mov	x9, x0
   82958:	52800061 	mov	w1, #0x3                   	// #3
   8295c:	17fffe2d 	b	82210 <init_kmalloc+0x160>
   82960:	52800032 	mov	w18, #0x1                   	// #1
   82964:	b900b7f2 	str	w18, [sp, #180]
   82968:	b900dbf2 	str	w18, [sp, #216]
   8296c:	aa1703f4 	mov	x20, x23
   82970:	aa0d03e9 	mov	x9, x13
   82974:	2a1203ef 	mov	w15, w18
   82978:	7100143f 	cmp	w1, #0x5
   8297c:	54ffc9a1 	b.ne	822b0 <init_kmalloc+0x200>  // b.any
   82980:	aa0303f7 	mov	x23, x3
   82984:	aa0003ed 	mov	x13, x0
   82988:	52800081 	mov	w1, #0x4                   	// #4
   8298c:	17fffe21 	b	82210 <init_kmalloc+0x160>
   82990:	52800035 	mov	w21, #0x1                   	// #1
   82994:	291b57f5 	stp	w21, w21, [sp, #216]
   82998:	aa1a03f7 	mov	x23, x26
   8299c:	aa1003ed 	mov	x13, x16
   829a0:	2a1503f2 	mov	w18, w21
   829a4:	7100183f 	cmp	w1, #0x6
   829a8:	54ffc941 	b.ne	822d0 <init_kmalloc+0x220>  // b.any
   829ac:	aa0303fa 	mov	x26, x3
   829b0:	aa0003f0 	mov	x16, x0
   829b4:	528000a1 	mov	w1, #0x5                   	// #5
   829b8:	17fffe16 	b	82210 <init_kmalloc+0x160>
   829bc:	52800039 	mov	w25, #0x1                   	// #1
   829c0:	b900dff9 	str	w25, [sp, #220]
   829c4:	b90103f9 	str	w25, [sp, #256]
   829c8:	aa1e03f0 	mov	x16, x30
   829cc:	2a1903f5 	mov	w21, w25
   829d0:	71001c3f 	cmp	w1, #0x7
   829d4:	f9403ffa 	ldr	x26, [sp, #120]
   829d8:	54ffc8c1 	b.ne	822f0 <init_kmalloc+0x240>  // b.any
   829dc:	aa0003fe 	mov	x30, x0
   829e0:	528000c1 	mov	w1, #0x6                   	// #6
   829e4:	f9003fe3 	str	x3, [sp, #120]
   829e8:	17fffe0a 	b	82210 <init_kmalloc+0x160>
   829ec:	f94043f9 	ldr	x25, [sp, #128]
   829f0:	5280003b 	mov	w27, #0x1                   	// #1
   829f4:	f9003ff9 	str	x25, [sp, #120]
   829f8:	aa1603fe 	mov	x30, x22
   829fc:	b90103fb 	str	w27, [sp, #256]
   82a00:	2a1b03f9 	mov	w25, w27
   82a04:	b90107fb 	str	w27, [sp, #260]
   82a08:	7100203f 	cmp	w1, #0x8
   82a0c:	54ffc821 	b.ne	82310 <init_kmalloc+0x260>  // b.any
   82a10:	aa0003f6 	mov	x22, x0
   82a14:	528000e1 	mov	w1, #0x7                   	// #7
   82a18:	f90043e3 	str	x3, [sp, #128]
   82a1c:	17fffdfd 	b	82210 <init_kmalloc+0x160>
   82a20:	f9404bf6 	ldr	x22, [sp, #144]
   82a24:	f90043f6 	str	x22, [sp, #128]
   82a28:	aa1803f6 	mov	x22, x24
   82a2c:	52800038 	mov	w24, #0x1                   	// #1
   82a30:	b90107f8 	str	w24, [sp, #260]
   82a34:	2a1803fb 	mov	w27, w24
   82a38:	b9010bf8 	str	w24, [sp, #264]
   82a3c:	7100243f 	cmp	w1, #0x9
   82a40:	b9010ff8 	str	w24, [sp, #268]
   82a44:	54ffc761 	b.ne	82330 <init_kmalloc+0x280>  // b.any
   82a48:	aa0003f8 	mov	x24, x0
   82a4c:	52800101 	mov	w1, #0x8                   	// #8
   82a50:	f9004be3 	str	x3, [sp, #144]
   82a54:	17fffdef 	b	82210 <init_kmalloc+0x160>
   82a58:	f94053f8 	ldr	x24, [sp, #160]
   82a5c:	f9004bf8 	str	x24, [sp, #144]
   82a60:	910443f8 	add	x24, sp, #0x110
   82a64:	5280003c 	mov	w28, #0x1                   	// #1
   82a68:	7100283f 	cmp	w1, #0xa
   82a6c:	293f731c 	stp	w28, w28, [x24, #-8]
   82a70:	2900731c 	stp	w28, w28, [x24]
   82a74:	f9403bf8 	ldr	x24, [sp, #112]
   82a78:	54ffc6e1 	b.ne	82354 <init_kmalloc+0x2a4>  // b.any
   82a7c:	52800121 	mov	w1, #0x9                   	// #9
   82a80:	f9003be0 	str	x0, [sp, #112]
   82a84:	f90053e3 	str	x3, [sp, #160]
   82a88:	17fffde2 	b	82210 <init_kmalloc+0x160>
   82a8c:	f9405ffc 	ldr	x28, [sp, #184]
   82a90:	f90053fc 	str	x28, [sp, #160]
   82a94:	f9404ffc 	ldr	x28, [sp, #152]
   82a98:	f9003bfc 	str	x28, [sp, #112]
   82a9c:	5280003c 	mov	w28, #0x1                   	// #1
   82aa0:	b90113fc 	str	w28, [sp, #272]
   82aa4:	b90117fc 	str	w28, [sp, #276]
   82aa8:	71002c3f 	cmp	w1, #0xb
   82aac:	b9011ffc 	str	w28, [sp, #284]
   82ab0:	b9012bfc 	str	w28, [sp, #296]
   82ab4:	54ffc621 	b.ne	82378 <init_kmalloc+0x2c8>  // b.any
   82ab8:	52800141 	mov	w1, #0xa                   	// #10
   82abc:	f9004fe0 	str	x0, [sp, #152]
   82ac0:	f9005fe3 	str	x3, [sp, #184]
   82ac4:	17fffdd3 	b	82210 <init_kmalloc+0x160>
   82ac8:	f9406bfc 	ldr	x28, [sp, #208]
   82acc:	f9005ffc 	str	x28, [sp, #184]
   82ad0:	f94057fc 	ldr	x28, [sp, #168]
   82ad4:	f9004ffc 	str	x28, [sp, #152]
   82ad8:	5280003c 	mov	w28, #0x1                   	// #1
   82adc:	b9011ffc 	str	w28, [sp, #284]
   82ae0:	b9012bfc 	str	w28, [sp, #296]
   82ae4:	7100303f 	cmp	w1, #0xc
   82ae8:	b9012ffc 	str	w28, [sp, #300]
   82aec:	b90133fc 	str	w28, [sp, #304]
   82af0:	54ffc561 	b.ne	8239c <init_kmalloc+0x2ec>  // b.any
   82af4:	52800161 	mov	w1, #0xb                   	// #11
   82af8:	f90057e0 	str	x0, [sp, #168]
   82afc:	f9006be3 	str	x3, [sp, #208]
   82b00:	17fffdc4 	b	82210 <init_kmalloc+0x160>
   82b04:	cb24c000 	sub	x0, x0, w4, sxtw
   82b08:	dac00008 	rbit	x8, x0
   82b0c:	eb03001f 	cmp	x0, x3
   82b10:	d34cfc04 	lsr	x4, x0, #12
   82b14:	dac01108 	clz	x8, x8
   82b18:	386468c2 	ldrb	w2, [x6, x4]
   82b1c:	12001042 	and	w2, w2, #0x1f
   82b20:	321b0442 	orr	w2, w2, #0x60
   82b24:	382468c2 	strb	w2, [x6, x4]
   82b28:	f94082a5 	ldr	x5, [x21, #256]
   82b2c:	386468a2 	ldrb	w2, [x5, x4]
   82b30:	121b6842 	and	w2, w2, #0xffffffe0
   82b34:	2a010041 	orr	w1, w2, w1
   82b38:	382468a1 	strb	w1, [x5, x4]
   82b3c:	54ffe3a1 	b.ne	827b0 <init_kmalloc+0x700>  // b.any
   82b40:	17ffff38 	b	82820 <init_kmalloc+0x770>
   82b44:	f94077fc 	ldr	x28, [sp, #232]
   82b48:	f9006bfc 	str	x28, [sp, #208]
   82b4c:	f94067fc 	ldr	x28, [sp, #200]
   82b50:	f90057fc 	str	x28, [sp, #168]
   82b54:	5280003c 	mov	w28, #0x1                   	// #1
   82b58:	b9012ffc 	str	w28, [sp, #300]
   82b5c:	b90133fc 	str	w28, [sp, #304]
   82b60:	7100343f 	cmp	w1, #0xd
   82b64:	b9013bfc 	str	w28, [sp, #312]
   82b68:	b9013ffc 	str	w28, [sp, #316]
   82b6c:	54ffc2a1 	b.ne	823c0 <init_kmalloc+0x310>  // b.any
   82b70:	52800181 	mov	w1, #0xc                   	// #12
   82b74:	f90067e0 	str	x0, [sp, #200]
   82b78:	f90077e3 	str	x3, [sp, #232]
   82b7c:	17fffda5 	b	82210 <init_kmalloc+0x160>
   82b80:	f9407ffc 	ldr	x28, [sp, #248]
   82b84:	f90077fc 	str	x28, [sp, #232]
   82b88:	f94063fc 	ldr	x28, [sp, #192]
   82b8c:	f90067fc 	str	x28, [sp, #200]
   82b90:	5280003c 	mov	w28, #0x1                   	// #1
   82b94:	b9011bfc 	str	w28, [sp, #280]
   82b98:	b90137fc 	str	w28, [sp, #308]
   82b9c:	7100383f 	cmp	w1, #0xe
   82ba0:	b9013bfc 	str	w28, [sp, #312]
   82ba4:	b9013ffc 	str	w28, [sp, #316]
   82ba8:	54ffc1e1 	b.ne	823e4 <init_kmalloc+0x334>  // b.any
   82bac:	528001a1 	mov	w1, #0xd                   	// #13
   82bb0:	f90063e0 	str	x0, [sp, #192]
   82bb4:	f9007fe3 	str	x3, [sp, #248]
   82bb8:	17fffd96 	b	82210 <init_kmalloc+0x160>
   82bbc:	f940a3fc 	ldr	x28, [sp, #320]
   82bc0:	f9007ffc 	str	x28, [sp, #248]
   82bc4:	f9407bfc 	ldr	x28, [sp, #240]
   82bc8:	51000421 	sub	w1, w1, #0x1
   82bcc:	f90063fc 	str	x28, [sp, #192]
   82bd0:	f9007be0 	str	x0, [sp, #240]
   82bd4:	52800020 	mov	w0, #0x1                   	// #1
   82bd8:	b9011be0 	str	w0, [sp, #280]
   82bdc:	b90137e0 	str	w0, [sp, #308]
   82be0:	f900a3e3 	str	x3, [sp, #320]
   82be4:	b9014be0 	str	w0, [sp, #328]
   82be8:	b9014fe0 	str	w0, [sp, #332]
   82bec:	17fffd89 	b	82210 <init_kmalloc+0x160>
   82bf0:	d2800000 	mov	x0, #0x0                   	// #0
   82bf4:	17ffff14 	b	82844 <init_kmalloc+0x794>
   82bf8:	a9446bf9 	ldp	x25, x26, [sp, #64]
   82bfc:	a94573fb 	ldp	x27, x28, [sp, #80]
   82c00:	17fffe8c 	b	82630 <init_kmalloc+0x580>
   82c04:	00000000 	.inst	0x00000000 ; undefined

0000000000082c08 <set>:
   82c08:	b9000001 	str	w1, [x0]
   82c0c:	d65f03c0 	ret

0000000000082c10 <reset>:
   82c10:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   82c14:	52800041 	mov	w1, #0x2                   	// #2
   82c18:	910003fd 	mov	x29, sp
   82c1c:	f9000bf3 	str	x19, [sp, #16]
   82c20:	2a0003f3 	mov	w19, w0
   82c24:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82c28:	91064000 	add	x0, x0, #0x190
   82c2c:	94000121 	bl	830b0 <log>
   82c30:	94000086 	bl	82e48 <flush>
   82c34:	d2800381 	mov	x1, #0x1c                  	// #28
   82c38:	d2800480 	mov	x0, #0x24                  	// #36
   82c3c:	f2a7e201 	movk	x1, #0x3f10, lsl #16
   82c40:	f2a7e200 	movk	x0, #0x3f10, lsl #16
   82c44:	52800402 	mov	w2, #0x20                  	// #32
   82c48:	72ab4002 	movk	w2, #0x5a00, lsl #16
   82c4c:	b9000022 	str	w2, [x1]
   82c50:	52ab4001 	mov	w1, #0x5a000000            	// #1509949440
   82c54:	2a010273 	orr	w19, w19, w1
   82c58:	b9000013 	str	w19, [x0]
   82c5c:	f9400bf3 	ldr	x19, [sp, #16]
   82c60:	a8c27bfd 	ldp	x29, x30, [sp], #32
   82c64:	d65f03c0 	ret

0000000000082c68 <cancel_reset>:
   82c68:	d2800382 	mov	x2, #0x1c                  	// #28
   82c6c:	d2800481 	mov	x1, #0x24                  	// #36
   82c70:	f2a7e202 	movk	x2, #0x3f10, lsl #16
   82c74:	f2a7e201 	movk	x1, #0x3f10, lsl #16
   82c78:	52ab4000 	mov	w0, #0x5a000000            	// #1509949440
   82c7c:	b9000040 	str	w0, [x2]
   82c80:	b9000020 	str	w0, [x1]
   82c84:	d65f03c0 	ret

0000000000082c88 <exc_not_imp>:
   82c88:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82c8c:	52800021 	mov	w1, #0x1                   	// #1
   82c90:	91066000 	add	x0, x0, #0x198
   82c94:	14000107 	b	830b0 <log>

0000000000082c98 <syn_handler>:
   82c98:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   82c9c:	52800002 	mov	w2, #0x0                   	// #0
   82ca0:	910003fd 	mov	x29, sp
   82ca4:	a90153f3 	stp	x19, x20, [sp, #16]
   82ca8:	f0000014 	adrp	x20, 85000 <el1_evt+0x1000>
   82cac:	9106e294 	add	x20, x20, #0x1b8
   82cb0:	aa0003f3 	mov	x19, x0
   82cb4:	a9025bf5 	stp	x21, x22, [sp, #32]
   82cb8:	aa0103f5 	mov	x21, x1
   82cbc:	aa0403f6 	mov	x22, x4
   82cc0:	aa0003e1 	mov	x1, x0
   82cc4:	aa1403e0 	mov	x0, x20
   82cc8:	9400020a 	bl	834f0 <log_hex>
   82ccc:	aa1403e0 	mov	x0, x20
   82cd0:	aa1503e1 	mov	x1, x21
   82cd4:	52800002 	mov	w2, #0x0                   	// #0
   82cd8:	94000206 	bl	834f0 <log_hex>
   82cdc:	d2800780 	mov	x0, #0x3c                  	// #60
   82ce0:	f2aac000 	movk	x0, #0x5600, lsl #16
   82ce4:	eb00027f 	cmp	x19, x0
   82ce8:	540000a0 	b.eq	82cfc <syn_handler+0x64>  // b.none
   82cec:	a94153f3 	ldp	x19, x20, [sp, #16]
   82cf0:	a9425bf5 	ldp	x21, x22, [sp, #32]
   82cf4:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82cf8:	d65f03c0 	ret
   82cfc:	aa1603e0 	mov	x0, x22
   82d00:	a94153f3 	ldp	x19, x20, [sp, #16]
   82d04:	a9425bf5 	ldp	x21, x22, [sp, #32]
   82d08:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82d0c:	17fff576 	b	802e4 <ret_kern>

0000000000082d10 <irq_handler>:
   82d10:	f0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   82d14:	52800001 	mov	w1, #0x0                   	// #0
   82d18:	91072000 	add	x0, x0, #0x1c8
   82d1c:	140000e5 	b	830b0 <log>

0000000000082d20 <uart_init>:
   82d20:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   82d24:	d28a0081 	mov	x1, #0x5004                	// #20484
   82d28:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82d2c:	910003fd 	mov	x29, sp
   82d30:	b9400020 	ldr	w0, [x1]
   82d34:	d28a0984 	mov	x4, #0x504c                	// #20556
   82d38:	a90153f3 	stp	x19, x20, [sp, #16]
   82d3c:	d28a0c13 	mov	x19, #0x5060                	// #20576
   82d40:	f2a7e433 	movk	x19, #0x3f21, lsl #16
   82d44:	f90013f5 	str	x21, [sp, #32]
   82d48:	32000000 	orr	w0, w0, #0x1
   82d4c:	f2a7e424 	movk	x4, #0x3f21, lsl #16
   82d50:	d28a0a05 	mov	x5, #0x5050                	// #20560
   82d54:	b9000020 	str	w0, [x1]
   82d58:	f2a7e425 	movk	x5, #0x3f21, lsl #16
   82d5c:	d28a0882 	mov	x2, #0x5044                	// #20548
   82d60:	b900027f 	str	wzr, [x19]
   82d64:	52800074 	mov	w20, #0x3                   	// #3
   82d68:	f2a7e422 	movk	x2, #0x3f21, lsl #16
   82d6c:	d28a0903 	mov	x3, #0x5048                	// #20552
   82d70:	b9000094 	str	w20, [x4]
   82d74:	f2a7e423 	movk	x3, #0x3f21, lsl #16
   82d78:	d28a0d00 	mov	x0, #0x5068                	// #20584
   82d7c:	b90000bf 	str	wzr, [x5]
   82d80:	f2a7e420 	movk	x0, #0x3f21, lsl #16
   82d84:	b900005f 	str	wzr, [x2]
   82d88:	528018c4 	mov	w4, #0xc6                  	// #198
   82d8c:	d2800082 	mov	x2, #0x4                   	// #4
   82d90:	b9000064 	str	w4, [x3]
   82d94:	f2a7e402 	movk	x2, #0x3f20, lsl #16
   82d98:	528021c1 	mov	w1, #0x10e                 	// #270
   82d9c:	b9000001 	str	w1, [x0]
   82da0:	d2801283 	mov	x3, #0x94                  	// #148
   82da4:	f2a7e403 	movk	x3, #0x3f20, lsl #16
   82da8:	52840004 	mov	w4, #0x2000                	// #8192
   82dac:	b9400041 	ldr	w1, [x2]
   82db0:	72a00024 	movk	w4, #0x1, lsl #16
   82db4:	d2801315 	mov	x21, #0x98                  	// #152
   82db8:	d28012c0 	mov	x0, #0x96                  	// #150
   82dbc:	120e6421 	and	w1, w1, #0xfffc0fff
   82dc0:	f2a7e415 	movk	x21, #0x3f20, lsl #16
   82dc4:	2a040021 	orr	w1, w1, w4
   82dc8:	b9000041 	str	w1, [x2]
   82dcc:	b900007f 	str	wzr, [x3]
   82dd0:	940001f6 	bl	835a8 <wait_clock>
   82dd4:	52980000 	mov	w0, #0xc000                	// #49152
   82dd8:	b90002a0 	str	w0, [x21]
   82ddc:	d28012c0 	mov	x0, #0x96                  	// #150
   82de0:	940001f2 	bl	835a8 <wait_clock>
   82de4:	b90002bf 	str	wzr, [x21]
   82de8:	f94013f5 	ldr	x21, [sp, #32]
   82dec:	b9000274 	str	w20, [x19]
   82df0:	a94153f3 	ldp	x19, x20, [sp, #16]
   82df4:	a8c37bfd 	ldp	x29, x30, [sp], #48
   82df8:	d65f03c0 	ret
   82dfc:	d503201f 	nop

0000000000082e00 <uart_recv_c>:
   82e00:	d28a0a81 	mov	x1, #0x5054                	// #20564
   82e04:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82e08:	b9400020 	ldr	w0, [x1]
   82e0c:	3607ffe0 	tbz	w0, #0, 82e08 <uart_recv_c+0x8>
   82e10:	d28a0800 	mov	x0, #0x5040                	// #20544
   82e14:	f2a7e420 	movk	x0, #0x3f21, lsl #16
   82e18:	b9400000 	ldr	w0, [x0]
   82e1c:	d65f03c0 	ret

0000000000082e20 <uart_send_c>:
   82e20:	d28a0a82 	mov	x2, #0x5054                	// #20564
   82e24:	12001c00 	and	w0, w0, #0xff
   82e28:	f2a7e422 	movk	x2, #0x3f21, lsl #16
   82e2c:	d503201f 	nop
   82e30:	b9400041 	ldr	w1, [x2]
   82e34:	362fffe1 	tbz	w1, #5, 82e30 <uart_send_c+0x10>
   82e38:	d28a0801 	mov	x1, #0x5040                	// #20544
   82e3c:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82e40:	b9000020 	str	w0, [x1]
   82e44:	d65f03c0 	ret

0000000000082e48 <flush>:
   82e48:	d28a0c81 	mov	x1, #0x5064                	// #20580
   82e4c:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82e50:	b9400020 	ldr	w0, [x1]
   82e54:	364fffe0 	tbz	w0, #9, 82e50 <flush+0x8>
   82e58:	d65f03c0 	ret
   82e5c:	d503201f 	nop

0000000000082e60 <uart_recv_i>:
   82e60:	d10043ff 	sub	sp, sp, #0x10
   82e64:	d28a0a81 	mov	x1, #0x5054                	// #20564
   82e68:	d28a0803 	mov	x3, #0x5040                	// #20544
   82e6c:	910033e2 	add	x2, sp, #0xc
   82e70:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82e74:	f2a7e423 	movk	x3, #0x3f21, lsl #16
   82e78:	91000442 	add	x2, x2, #0x1
   82e7c:	d503201f 	nop
   82e80:	b9400020 	ldr	w0, [x1]
   82e84:	3607ffe0 	tbz	w0, #0, 82e80 <uart_recv_i+0x20>
   82e88:	b9400060 	ldr	w0, [x3]
   82e8c:	910043e4 	add	x4, sp, #0x10
   82e90:	381ff040 	sturb	w0, [x2, #-1]
   82e94:	eb04005f 	cmp	x2, x4
   82e98:	54ffff01 	b.ne	82e78 <uart_recv_i+0x18>  // b.any
   82e9c:	b9400fe0 	ldr	w0, [sp, #12]
   82ea0:	910043ff 	add	sp, sp, #0x10
   82ea4:	d65f03c0 	ret

0000000000082ea8 <uart_recv_l>:
   82ea8:	d10043ff 	sub	sp, sp, #0x10
   82eac:	d28a0a81 	mov	x1, #0x5054                	// #20564
   82eb0:	d28a0803 	mov	x3, #0x5040                	// #20544
   82eb4:	910023e2 	add	x2, sp, #0x8
   82eb8:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82ebc:	f2a7e423 	movk	x3, #0x3f21, lsl #16
   82ec0:	91000442 	add	x2, x2, #0x1
   82ec4:	d503201f 	nop
   82ec8:	b9400020 	ldr	w0, [x1]
   82ecc:	3607ffe0 	tbz	w0, #0, 82ec8 <uart_recv_l+0x20>
   82ed0:	b9400060 	ldr	w0, [x3]
   82ed4:	910043e4 	add	x4, sp, #0x10
   82ed8:	381ff040 	sturb	w0, [x2, #-1]
   82edc:	eb04005f 	cmp	x2, x4
   82ee0:	54ffff01 	b.ne	82ec0 <uart_recv_l+0x18>  // b.any
   82ee4:	f94007e0 	ldr	x0, [sp, #8]
   82ee8:	910043ff 	add	sp, sp, #0x10
   82eec:	d65f03c0 	ret

0000000000082ef0 <uart_send_i>:
   82ef0:	d10043ff 	sub	sp, sp, #0x10
   82ef4:	d28a0a81 	mov	x1, #0x5054                	// #20564
   82ef8:	d28a0804 	mov	x4, #0x5040                	// #20544
   82efc:	910033e2 	add	x2, sp, #0xc
   82f00:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82f04:	f2a7e424 	movk	x4, #0x3f21, lsl #16
   82f08:	b9000fe0 	str	w0, [sp, #12]
   82f0c:	38401443 	ldrb	w3, [x2], #1
   82f10:	b9400020 	ldr	w0, [x1]
   82f14:	362fffe0 	tbz	w0, #5, 82f10 <uart_send_i+0x20>
   82f18:	b9000083 	str	w3, [x4]
   82f1c:	910043e0 	add	x0, sp, #0x10
   82f20:	eb00005f 	cmp	x2, x0
   82f24:	54ffff41 	b.ne	82f0c <uart_send_i+0x1c>  // b.any
   82f28:	910043ff 	add	sp, sp, #0x10
   82f2c:	d65f03c0 	ret

0000000000082f30 <uart_send_l>:
   82f30:	d10043ff 	sub	sp, sp, #0x10
   82f34:	d28a0a81 	mov	x1, #0x5054                	// #20564
   82f38:	d28a0804 	mov	x4, #0x5040                	// #20544
   82f3c:	910023e2 	add	x2, sp, #0x8
   82f40:	f2a7e421 	movk	x1, #0x3f21, lsl #16
   82f44:	f2a7e424 	movk	x4, #0x3f21, lsl #16
   82f48:	f90007e0 	str	x0, [sp, #8]
   82f4c:	d503201f 	nop
   82f50:	38401443 	ldrb	w3, [x2], #1
   82f54:	d503201f 	nop
   82f58:	b9400020 	ldr	w0, [x1]
   82f5c:	362fffe0 	tbz	w0, #5, 82f58 <uart_send_l+0x28>
   82f60:	b9000083 	str	w3, [x4]
   82f64:	910043e0 	add	x0, sp, #0x10
   82f68:	eb00005f 	cmp	x2, x0
   82f6c:	54ffff21 	b.ne	82f50 <uart_send_l+0x20>  // b.any
   82f70:	910043ff 	add	sp, sp, #0x10
   82f74:	d65f03c0 	ret

0000000000082f78 <putc>:
   82f78:	17ffffaa 	b	82e20 <uart_send_c>
   82f7c:	d503201f 	nop

0000000000082f80 <puts>:
   82f80:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   82f84:	910003fd 	mov	x29, sp
   82f88:	f9000bf3 	str	x19, [sp, #16]
   82f8c:	aa0003f3 	mov	x19, x0
   82f90:	39400000 	ldrb	w0, [x0]
   82f94:	34000080 	cbz	w0, 82fa4 <puts+0x24>
   82f98:	97ffffa2 	bl	82e20 <uart_send_c>
   82f9c:	38401e60 	ldrb	w0, [x19, #1]!
   82fa0:	35ffffc0 	cbnz	w0, 82f98 <puts+0x18>
   82fa4:	f9400bf3 	ldr	x19, [sp, #16]
   82fa8:	52800140 	mov	w0, #0xa                   	// #10
   82fac:	a8c27bfd 	ldp	x29, x30, [sp], #32
   82fb0:	17ffff9c 	b	82e20 <uart_send_c>
   82fb4:	d503201f 	nop

0000000000082fb8 <puts_n>:
   82fb8:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   82fbc:	f100003f 	cmp	x1, #0x0
   82fc0:	910003fd 	mov	x29, sp
   82fc4:	a90153f3 	stp	x19, x20, [sp, #16]
   82fc8:	aa0003f4 	mov	x20, x0
   82fcc:	39400000 	ldrb	w0, [x0]
   82fd0:	7a401804 	ccmp	w0, #0x0, #0x4, ne  // ne = any
   82fd4:	54000180 	b.eq	83004 <puts_n+0x4c>  // b.none
   82fd8:	f90013f5 	str	x21, [sp, #32]
   82fdc:	aa0103f5 	mov	x21, x1
   82fe0:	d2800013 	mov	x19, #0x0                   	// #0
   82fe4:	d503201f 	nop
   82fe8:	91000673 	add	x19, x19, #0x1
   82fec:	97ffff8d 	bl	82e20 <uart_send_c>
   82ff0:	38736a80 	ldrb	w0, [x20, x19]
   82ff4:	7100001f 	cmp	w0, #0x0
   82ff8:	fa5312a0 	ccmp	x21, x19, #0x0, ne  // ne = any
   82ffc:	54ffff68 	b.hi	82fe8 <puts_n+0x30>  // b.pmore
   83000:	f94013f5 	ldr	x21, [sp, #32]
   83004:	52800140 	mov	w0, #0xa                   	// #10
   83008:	a94153f3 	ldp	x19, x20, [sp, #16]
   8300c:	a8c37bfd 	ldp	x29, x30, [sp], #48
   83010:	17ffff84 	b	82e20 <uart_send_c>
   83014:	d503201f 	nop

0000000000083018 <getc>:
   83018:	17ffff7a 	b	82e00 <uart_recv_c>
   8301c:	d503201f 	nop

0000000000083020 <gets>:
   83020:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   83024:	910003fd 	mov	x29, sp
   83028:	f9000bf3 	str	x19, [sp, #16]
   8302c:	aa0003f3 	mov	x19, x0
   83030:	14000002 	b	83038 <gets+0x18>
   83034:	38001660 	strb	w0, [x19], #1
   83038:	97ffff72 	bl	82e00 <uart_recv_c>
   8303c:	12001c01 	and	w1, w0, #0xff
   83040:	7100283f 	cmp	w1, #0xa
   83044:	54ffff81 	b.ne	83034 <gets+0x14>  // b.any
   83048:	3900027f 	strb	wzr, [x19]
   8304c:	f9400bf3 	ldr	x19, [sp, #16]
   83050:	a8c27bfd 	ldp	x29, x30, [sp], #32
   83054:	d65f03c0 	ret

0000000000083058 <gets_n>:
   83058:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   8305c:	910003fd 	mov	x29, sp
   83060:	a90153f3 	stp	x19, x20, [sp, #16]
   83064:	aa0003f4 	mov	x20, x0
   83068:	d2800013 	mov	x19, #0x0                   	// #0
   8306c:	f90013f5 	str	x21, [sp, #32]
   83070:	aa0103f5 	mov	x21, x1
   83074:	14000003 	b	83080 <gets_n+0x28>
   83078:	38336a80 	strb	w0, [x20, x19]
   8307c:	91000673 	add	x19, x19, #0x1
   83080:	97ffff60 	bl	82e00 <uart_recv_c>
   83084:	12001c02 	and	w2, w0, #0xff
   83088:	eb1302bf 	cmp	x21, x19
   8308c:	8b130281 	add	x1, x20, x19
   83090:	7a4a8844 	ccmp	w2, #0xa, #0x4, hi  // hi = pmore
   83094:	54ffff21 	b.ne	83078 <gets_n+0x20>  // b.any
   83098:	3900003f 	strb	wzr, [x1]
   8309c:	a94153f3 	ldp	x19, x20, [sp, #16]
   830a0:	f94013f5 	ldr	x21, [sp, #32]
   830a4:	a8c37bfd 	ldp	x29, x30, [sp], #48
   830a8:	d65f03c0 	ret
   830ac:	d503201f 	nop

00000000000830b0 <log>:
   830b0:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   830b4:	910003fd 	mov	x29, sp
   830b8:	f9000bf3 	str	x19, [sp, #16]
   830bc:	aa0003f3 	mov	x19, x0
   830c0:	35000141 	cbnz	w1, 830e8 <log+0x38>
   830c4:	39400000 	ldrb	w0, [x0]
   830c8:	340000a0 	cbz	w0, 830dc <log+0x2c>
   830cc:	d503201f 	nop
   830d0:	97ffff54 	bl	82e20 <uart_send_c>
   830d4:	38401e60 	ldrb	w0, [x19, #1]!
   830d8:	35ffffc0 	cbnz	w0, 830d0 <log+0x20>
   830dc:	f9400bf3 	ldr	x19, [sp, #16]
   830e0:	a8c27bfd 	ldp	x29, x30, [sp], #32
   830e4:	d65f03c0 	ret
   830e8:	7100043f 	cmp	w1, #0x1
   830ec:	54000120 	b.eq	83110 <log+0x60>  // b.none
   830f0:	7100083f 	cmp	w1, #0x2
   830f4:	54ffff41 	b.ne	830dc <log+0x2c>  // b.any
   830f8:	39400000 	ldrb	w0, [x0]
   830fc:	34ffff00 	cbz	w0, 830dc <log+0x2c>
   83100:	97ffff48 	bl	82e20 <uart_send_c>
   83104:	38401e60 	ldrb	w0, [x19, #1]!
   83108:	35ffffc0 	cbnz	w0, 83100 <log+0x50>
   8310c:	17fffff4 	b	830dc <log+0x2c>
   83110:	39400000 	ldrb	w0, [x0]
   83114:	34fffe40 	cbz	w0, 830dc <log+0x2c>
   83118:	97ffff42 	bl	82e20 <uart_send_c>
   8311c:	38401e60 	ldrb	w0, [x19, #1]!
   83120:	35ffffc0 	cbnz	w0, 83118 <log+0x68>
   83124:	f9400bf3 	ldr	x19, [sp, #16]
   83128:	a8c27bfd 	ldp	x29, x30, [sp], #32
   8312c:	d65f03c0 	ret

0000000000083130 <print>:
   83130:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   83134:	910003fd 	mov	x29, sp
   83138:	f9000bf3 	str	x19, [sp, #16]
   8313c:	aa0003f3 	mov	x19, x0
   83140:	39400000 	ldrb	w0, [x0]
   83144:	34000080 	cbz	w0, 83154 <print+0x24>
   83148:	97ffff36 	bl	82e20 <uart_send_c>
   8314c:	38401e60 	ldrb	w0, [x19, #1]!
   83150:	35ffffc0 	cbnz	w0, 83148 <print+0x18>
   83154:	f9400bf3 	ldr	x19, [sp, #16]
   83158:	a8c27bfd 	ldp	x29, x30, [sp], #32
   8315c:	d65f03c0 	ret

0000000000083160 <print_n>:
   83160:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
   83164:	f100003f 	cmp	x1, #0x0
   83168:	910003fd 	mov	x29, sp
   8316c:	a90153f3 	stp	x19, x20, [sp, #16]
   83170:	aa0003f4 	mov	x20, x0
   83174:	39400000 	ldrb	w0, [x0]
   83178:	7a401804 	ccmp	w0, #0x0, #0x4, ne  // ne = any
   8317c:	54000180 	b.eq	831ac <print_n+0x4c>  // b.none
   83180:	f90013f5 	str	x21, [sp, #32]
   83184:	aa0103f5 	mov	x21, x1
   83188:	d2800013 	mov	x19, #0x0                   	// #0
   8318c:	d503201f 	nop
   83190:	91000673 	add	x19, x19, #0x1
   83194:	97ffff23 	bl	82e20 <uart_send_c>
   83198:	38736a80 	ldrb	w0, [x20, x19]
   8319c:	7100001f 	cmp	w0, #0x0
   831a0:	fa5312a0 	ccmp	x21, x19, #0x0, ne  // ne = any
   831a4:	54ffff68 	b.hi	83190 <print_n+0x30>  // b.pmore
   831a8:	f94013f5 	ldr	x21, [sp, #32]
   831ac:	a94153f3 	ldp	x19, x20, [sp, #16]
   831b0:	a8c37bfd 	ldp	x29, x30, [sp], #48
   831b4:	d65f03c0 	ret

00000000000831b8 <print_hex>:
   831b8:	a9bc7bfd 	stp	x29, x30, [sp, #-64]!
   831bc:	910003fd 	mov	x29, sp
   831c0:	a90153f3 	stp	x19, x20, [sp, #16]
   831c4:	d0000014 	adrp	x20, 85000 <el1_evt+0x1000>
   831c8:	91076294 	add	x20, x20, #0x1d8
   831cc:	aa0003f3 	mov	x19, x0
   831d0:	52800600 	mov	w0, #0x30                  	// #48
   831d4:	d503201f 	nop
   831d8:	97ffff12 	bl	82e20 <uart_send_c>
   831dc:	38401e80 	ldrb	w0, [x20, #1]!
   831e0:	35ffffc0 	cbnz	w0, 831d8 <print_hex+0x20>
   831e4:	b4001273 	cbz	x19, 83430 <print_hex+0x278>
   831e8:	12000e60 	and	w0, w19, #0xf
   831ec:	53041e61 	ubfx	w1, w19, #4, #4
   831f0:	7100241f 	cmp	w0, #0x9
   831f4:	1100c00c 	add	w12, w0, #0x30
   831f8:	11015c00 	add	w0, w0, #0x57
   831fc:	1100c022 	add	w2, w1, #0x30
   83200:	1a8c800c 	csel	w12, w0, w12, hi  // hi = pmore
   83204:	11015c2d 	add	w13, w1, #0x57
   83208:	53082e60 	ubfx	w0, w19, #8, #4
   8320c:	7100243f 	cmp	w1, #0x9
   83210:	1a8281ad 	csel	w13, w13, w2, hi  // hi = pmore
   83214:	1100c001 	add	w1, w0, #0x30
   83218:	7100241f 	cmp	w0, #0x9
   8321c:	11015c0b 	add	w11, w0, #0x57
   83220:	530c3e60 	ubfx	w0, w19, #12, #4
   83224:	1a81816b 	csel	w11, w11, w1, hi  // hi = pmore
   83228:	11015c0a 	add	w10, w0, #0x57
   8322c:	1100c001 	add	w1, w0, #0x30
   83230:	7100241f 	cmp	w0, #0x9
   83234:	53104e60 	ubfx	w0, w19, #16, #4
   83238:	1a81814a 	csel	w10, w10, w1, hi  // hi = pmore
   8323c:	11015c09 	add	w9, w0, #0x57
   83240:	1100c001 	add	w1, w0, #0x30
   83244:	7100241f 	cmp	w0, #0x9
   83248:	53145e60 	ubfx	w0, w19, #20, #4
   8324c:	1a818129 	csel	w9, w9, w1, hi  // hi = pmore
   83250:	11015c08 	add	w8, w0, #0x57
   83254:	1100c001 	add	w1, w0, #0x30
   83258:	7100241f 	cmp	w0, #0x9
   8325c:	53186e60 	ubfx	w0, w19, #24, #4
   83260:	1a818108 	csel	w8, w8, w1, hi  // hi = pmore
   83264:	11015c07 	add	w7, w0, #0x57
   83268:	1100c001 	add	w1, w0, #0x30
   8326c:	7100241f 	cmp	w0, #0x9
   83270:	531c7e60 	lsr	w0, w19, #28
   83274:	1a8180e7 	csel	w7, w7, w1, hi  // hi = pmore
   83278:	11015c06 	add	w6, w0, #0x57
   8327c:	1100c001 	add	w1, w0, #0x30
   83280:	7100241f 	cmp	w0, #0x9
   83284:	d3608e60 	ubfx	x0, x19, #32, #4
   83288:	1a8180c6 	csel	w6, w6, w1, hi  // hi = pmore
   8328c:	11015c0e 	add	w14, w0, #0x57
   83290:	1100c001 	add	w1, w0, #0x30
   83294:	7100241f 	cmp	w0, #0x9
   83298:	d3649e60 	ubfx	x0, x19, #36, #4
   8329c:	1a8181ce 	csel	w14, w14, w1, hi  // hi = pmore
   832a0:	11015c05 	add	w5, w0, #0x57
   832a4:	1100c001 	add	w1, w0, #0x30
   832a8:	7100241f 	cmp	w0, #0x9
   832ac:	d368ae60 	ubfx	x0, x19, #40, #4
   832b0:	1a8180a5 	csel	w5, w5, w1, hi  // hi = pmore
   832b4:	11015c04 	add	w4, w0, #0x57
   832b8:	1100c001 	add	w1, w0, #0x30
   832bc:	7100241f 	cmp	w0, #0x9
   832c0:	d36cbe60 	ubfx	x0, x19, #44, #4
   832c4:	1a818084 	csel	w4, w4, w1, hi  // hi = pmore
   832c8:	11015c03 	add	w3, w0, #0x57
   832cc:	1100c001 	add	w1, w0, #0x30
   832d0:	7100241f 	cmp	w0, #0x9
   832d4:	d370ce60 	ubfx	x0, x19, #48, #4
   832d8:	1a818063 	csel	w3, w3, w1, hi  // hi = pmore
   832dc:	11015c02 	add	w2, w0, #0x57
   832e0:	1100c001 	add	w1, w0, #0x30
   832e4:	7100241f 	cmp	w0, #0x9
   832e8:	d374de60 	ubfx	x0, x19, #52, #4
   832ec:	1a818042 	csel	w2, w2, w1, hi  // hi = pmore
   832f0:	1100c00f 	add	w15, w0, #0x30
   832f4:	7100241f 	cmp	w0, #0x9
   832f8:	11015c01 	add	w1, w0, #0x57
   832fc:	d378ee60 	ubfx	x0, x19, #56, #4
   83300:	1a8f8021 	csel	w1, w1, w15, hi  // hi = pmore
   83304:	7100241f 	cmp	w0, #0x9
   83308:	1100c00f 	add	w15, w0, #0x30
   8330c:	11015c00 	add	w0, w0, #0x57
   83310:	1a8f8000 	csel	w0, w0, w15, hi  // hi = pmore
   83314:	3900a7e0 	strb	w0, [sp, #41]
   83318:	3900abe1 	strb	w1, [sp, #42]
   8331c:	d37cfe73 	lsr	x19, x19, #60
   83320:	3900afe2 	strb	w2, [sp, #43]
   83324:	7100267f 	cmp	w19, #0x9
   83328:	3900b3e3 	strb	w3, [sp, #44]
   8332c:	3900b7e4 	strb	w4, [sp, #45]
   83330:	3900bbe5 	strb	w5, [sp, #46]
   83334:	3900bfee 	strb	w14, [sp, #47]
   83338:	3900c3e6 	strb	w6, [sp, #48]
   8333c:	3900c7e7 	strb	w7, [sp, #49]
   83340:	3900cbe8 	strb	w8, [sp, #50]
   83344:	3900cfe9 	strb	w9, [sp, #51]
   83348:	3900d3ea 	strb	w10, [sp, #52]
   8334c:	3900d7eb 	strb	w11, [sp, #53]
   83350:	3900dbed 	strb	w13, [sp, #54]
   83354:	3900dfec 	strb	w12, [sp, #55]
   83358:	54000528 	b.hi	833fc <print_hex+0x244>  // b.pmore
   8335c:	1100c273 	add	w19, w19, #0x30
   83360:	3900a3f3 	strb	w19, [sp, #40]
   83364:	3900e3ff 	strb	wzr, [sp, #56]
   83368:	7100c27f 	cmp	w19, #0x30
   8336c:	54000bc1 	b.ne	834e4 <print_hex+0x32c>  // b.any
   83370:	7100c01f 	cmp	w0, #0x30
   83374:	540006c1 	b.ne	8344c <print_hex+0x294>  // b.any
   83378:	7100c03f 	cmp	w1, #0x30
   8337c:	540006c1 	b.ne	83454 <print_hex+0x29c>  // b.any
   83380:	7100c05f 	cmp	w2, #0x30
   83384:	540006e1 	b.ne	83460 <print_hex+0x2a8>  // b.any
   83388:	7100c07f 	cmp	w3, #0x30
   8338c:	54000701 	b.ne	8346c <print_hex+0x2b4>  // b.any
   83390:	7100c09f 	cmp	w4, #0x30
   83394:	54000721 	b.ne	83478 <print_hex+0x2c0>  // b.any
   83398:	7100c0bf 	cmp	w5, #0x30
   8339c:	54000741 	b.ne	83484 <print_hex+0x2cc>  // b.any
   833a0:	7100c1df 	cmp	w14, #0x30
   833a4:	54000761 	b.ne	83490 <print_hex+0x2d8>  // b.any
   833a8:	7100c0df 	cmp	w6, #0x30
   833ac:	540004a1 	b.ne	83440 <print_hex+0x288>  // b.any
   833b0:	7100c0ff 	cmp	w7, #0x30
   833b4:	54000741 	b.ne	8349c <print_hex+0x2e4>  // b.any
   833b8:	7100c11f 	cmp	w8, #0x30
   833bc:	54000761 	b.ne	834a8 <print_hex+0x2f0>  // b.any
   833c0:	7100c13f 	cmp	w9, #0x30
   833c4:	54000781 	b.ne	834b4 <print_hex+0x2fc>  // b.any
   833c8:	7100c15f 	cmp	w10, #0x30
   833cc:	54000801 	b.ne	834cc <print_hex+0x314>  // b.any
   833d0:	7100c17f 	cmp	w11, #0x30
   833d4:	54000761 	b.ne	834c0 <print_hex+0x308>  // b.any
   833d8:	7100c1bf 	cmp	w13, #0x30
   833dc:	540007e1 	b.ne	834d8 <print_hex+0x320>  // b.any
   833e0:	2a0c03e0 	mov	w0, w12
   833e4:	7100c19f 	cmp	w12, #0x30
   833e8:	528001f3 	mov	w19, #0xf                   	// #15
   833ec:	54000121 	b.ne	83410 <print_hex+0x258>  // b.any
   833f0:	a94153f3 	ldp	x19, x20, [sp, #16]
   833f4:	a8c47bfd 	ldp	x29, x30, [sp], #64
   833f8:	d65f03c0 	ret
   833fc:	11015e73 	add	w19, w19, #0x57
   83400:	3900e3ff 	strb	wzr, [sp, #56]
   83404:	12001e60 	and	w0, w19, #0xff
   83408:	52800013 	mov	w19, #0x0                   	// #0
   8340c:	3900a3e0 	strb	w0, [sp, #40]
   83410:	9100a3e1 	add	x1, sp, #0x28
   83414:	8b33c033 	add	x19, x1, w19, sxtw
   83418:	97fffe82 	bl	82e20 <uart_send_c>
   8341c:	38401e60 	ldrb	w0, [x19, #1]!
   83420:	35ffffc0 	cbnz	w0, 83418 <print_hex+0x260>
   83424:	a94153f3 	ldp	x19, x20, [sp, #16]
   83428:	a8c47bfd 	ldp	x29, x30, [sp], #64
   8342c:	d65f03c0 	ret
   83430:	a94153f3 	ldp	x19, x20, [sp, #16]
   83434:	52800600 	mov	w0, #0x30                  	// #48
   83438:	a8c47bfd 	ldp	x29, x30, [sp], #64
   8343c:	17fffe79 	b	82e20 <uart_send_c>
   83440:	2a0603e0 	mov	w0, w6
   83444:	52800113 	mov	w19, #0x8                   	// #8
   83448:	17fffff2 	b	83410 <print_hex+0x258>
   8344c:	52800033 	mov	w19, #0x1                   	// #1
   83450:	17fffff0 	b	83410 <print_hex+0x258>
   83454:	2a0103e0 	mov	w0, w1
   83458:	52800053 	mov	w19, #0x2                   	// #2
   8345c:	17ffffed 	b	83410 <print_hex+0x258>
   83460:	2a0203e0 	mov	w0, w2
   83464:	52800073 	mov	w19, #0x3                   	// #3
   83468:	17ffffea 	b	83410 <print_hex+0x258>
   8346c:	2a0303e0 	mov	w0, w3
   83470:	52800093 	mov	w19, #0x4                   	// #4
   83474:	17ffffe7 	b	83410 <print_hex+0x258>
   83478:	2a0403e0 	mov	w0, w4
   8347c:	528000b3 	mov	w19, #0x5                   	// #5
   83480:	17ffffe4 	b	83410 <print_hex+0x258>
   83484:	2a0503e0 	mov	w0, w5
   83488:	528000d3 	mov	w19, #0x6                   	// #6
   8348c:	17ffffe1 	b	83410 <print_hex+0x258>
   83490:	2a0e03e0 	mov	w0, w14
   83494:	528000f3 	mov	w19, #0x7                   	// #7
   83498:	17ffffde 	b	83410 <print_hex+0x258>
   8349c:	2a0703e0 	mov	w0, w7
   834a0:	52800133 	mov	w19, #0x9                   	// #9
   834a4:	17ffffdb 	b	83410 <print_hex+0x258>
   834a8:	2a0803e0 	mov	w0, w8
   834ac:	52800153 	mov	w19, #0xa                   	// #10
   834b0:	17ffffd8 	b	83410 <print_hex+0x258>
   834b4:	2a0903e0 	mov	w0, w9
   834b8:	52800173 	mov	w19, #0xb                   	// #11
   834bc:	17ffffd5 	b	83410 <print_hex+0x258>
   834c0:	2a0b03e0 	mov	w0, w11
   834c4:	528001b3 	mov	w19, #0xd                   	// #13
   834c8:	17ffffd2 	b	83410 <print_hex+0x258>
   834cc:	2a0a03e0 	mov	w0, w10
   834d0:	52800193 	mov	w19, #0xc                   	// #12
   834d4:	17ffffcf 	b	83410 <print_hex+0x258>
   834d8:	2a0d03e0 	mov	w0, w13
   834dc:	528001d3 	mov	w19, #0xe                   	// #14
   834e0:	17ffffcc 	b	83410 <print_hex+0x258>
   834e4:	2a1303e0 	mov	w0, w19
   834e8:	52800013 	mov	w19, #0x0                   	// #0
   834ec:	17ffffc9 	b	83410 <print_hex+0x258>

00000000000834f0 <log_hex>:
   834f0:	a9be7bfd 	stp	x29, x30, [sp, #-32]!
   834f4:	910003fd 	mov	x29, sp
   834f8:	a90153f3 	stp	x19, x20, [sp, #16]
   834fc:	aa0003f3 	mov	x19, x0
   83500:	aa0103f4 	mov	x20, x1
   83504:	350001c2 	cbnz	w2, 8353c <log_hex+0x4c>
   83508:	39400000 	ldrb	w0, [x0]
   8350c:	34000080 	cbz	w0, 8351c <log_hex+0x2c>
   83510:	97fffe44 	bl	82e20 <uart_send_c>
   83514:	38401e60 	ldrb	w0, [x19, #1]!
   83518:	35ffffc0 	cbnz	w0, 83510 <log_hex+0x20>
   8351c:	52800400 	mov	w0, #0x20                  	// #32
   83520:	97fffe40 	bl	82e20 <uart_send_c>
   83524:	aa1403e0 	mov	x0, x20
   83528:	97ffff24 	bl	831b8 <print_hex>
   8352c:	a94153f3 	ldp	x19, x20, [sp, #16]
   83530:	52800140 	mov	w0, #0xa                   	// #10
   83534:	a8c27bfd 	ldp	x29, x30, [sp], #32
   83538:	17fffe3a 	b	82e20 <uart_send_c>
   8353c:	7100045f 	cmp	w2, #0x1
   83540:	540000c0 	b.eq	83558 <log_hex+0x68>  // b.none
   83544:	7100085f 	cmp	w2, #0x2
   83548:	54000220 	b.eq	8358c <log_hex+0x9c>  // b.none
   8354c:	a94153f3 	ldp	x19, x20, [sp, #16]
   83550:	a8c27bfd 	ldp	x29, x30, [sp], #32
   83554:	d65f03c0 	ret
   83558:	39400000 	ldrb	w0, [x0]
   8355c:	34fffe00 	cbz	w0, 8351c <log_hex+0x2c>
   83560:	97fffe30 	bl	82e20 <uart_send_c>
   83564:	38401e60 	ldrb	w0, [x19, #1]!
   83568:	35ffffc0 	cbnz	w0, 83560 <log_hex+0x70>
   8356c:	52800400 	mov	w0, #0x20                  	// #32
   83570:	97fffe2c 	bl	82e20 <uart_send_c>
   83574:	aa1403e0 	mov	x0, x20
   83578:	97ffff10 	bl	831b8 <print_hex>
   8357c:	a94153f3 	ldp	x19, x20, [sp, #16]
   83580:	52800140 	mov	w0, #0xa                   	// #10
   83584:	a8c27bfd 	ldp	x29, x30, [sp], #32
   83588:	17fffe26 	b	82e20 <uart_send_c>
   8358c:	39400000 	ldrb	w0, [x0]
   83590:	34fffc60 	cbz	w0, 8351c <log_hex+0x2c>
   83594:	d503201f 	nop
   83598:	97fffe22 	bl	82e20 <uart_send_c>
   8359c:	38401e60 	ldrb	w0, [x19, #1]!
   835a0:	35ffffc0 	cbnz	w0, 83598 <log_hex+0xa8>
   835a4:	17ffffde 	b	8351c <log_hex+0x2c>

00000000000835a8 <wait_clock>:
   835a8:	d342fc00 	lsr	x0, x0, #2
   835ac:	d1000401 	sub	x1, x0, #0x1
   835b0:	b40000a0 	cbz	x0, 835c4 <wait_clock+0x1c>
   835b4:	d503201f 	nop
   835b8:	d1000421 	sub	x1, x1, #0x1
   835bc:	b100043f 	cmn	x1, #0x1
   835c0:	54ffffc1 	b.ne	835b8 <wait_clock+0x10>  // b.any
   835c4:	d65f03c0 	ret

00000000000835c8 <strcmp>:
   835c8:	39400003 	ldrb	w3, [x0]
   835cc:	d2800022 	mov	x2, #0x1                   	// #1
   835d0:	385ff424 	ldrb	w4, [x1], #-1
   835d4:	350000c3 	cbnz	w3, 835ec <strcmp+0x24>
   835d8:	14000008 	b	835f8 <strcmp+0x30>
   835dc:	38626803 	ldrb	w3, [x0, x2]
   835e0:	91000442 	add	x2, x2, #0x1
   835e4:	38626824 	ldrb	w4, [x1, x2]
   835e8:	340000c3 	cbz	w3, 83600 <strcmp+0x38>
   835ec:	38626824 	ldrb	w4, [x1, x2]
   835f0:	6b03009f 	cmp	w4, w3
   835f4:	54ffff40 	b.eq	835dc <strcmp+0x14>  // b.none
   835f8:	4b040060 	sub	w0, w3, w4
   835fc:	d65f03c0 	ret
   83600:	52800003 	mov	w3, #0x0                   	// #0
   83604:	4b040060 	sub	w0, w3, w4
   83608:	d65f03c0 	ret
   8360c:	d503201f 	nop

0000000000083610 <strcmp_n>:
   83610:	39400003 	ldrb	w3, [x0]
   83614:	aa0003e4 	mov	x4, x0
   83618:	39400025 	ldrb	w5, [x1]
   8361c:	f1000442 	subs	x2, x2, #0x1
   83620:	54000101 	b.ne	83640 <strcmp_n+0x30>  // b.any
   83624:	1400000b 	b	83650 <strcmp_n+0x40>
   83628:	54000141 	b.ne	83650 <strcmp_n+0x40>  // b.any
   8362c:	38401c83 	ldrb	w3, [x4, #1]!
   83630:	38401c25 	ldrb	w5, [x1, #1]!
   83634:	cb040006 	sub	x6, x0, x4
   83638:	ab0200df 	cmn	x6, x2
   8363c:	540000a0 	b.eq	83650 <strcmp_n+0x40>  // b.none
   83640:	39400083 	ldrb	w3, [x4]
   83644:	39400025 	ldrb	w5, [x1]
   83648:	6b05007f 	cmp	w3, w5
   8364c:	35fffee3 	cbnz	w3, 83628 <strcmp_n+0x18>
   83650:	4b050060 	sub	w0, w3, w5
   83654:	d65f03c0 	ret

0000000000083658 <memcpy>:
   83658:	d1000444 	sub	x4, x2, #0x1
   8365c:	b40009a2 	cbz	x2, 83790 <memcpy+0x138>
   83660:	91003c03 	add	x3, x0, #0xf
   83664:	cb010063 	sub	x3, x3, x1
   83668:	f100787f 	cmp	x3, #0x1e
   8366c:	fa4e8880 	ccmp	x4, #0xe, #0x0, hi  // hi = pmore
   83670:	54000929 	b.ls	83794 <memcpy+0x13c>  // b.plast
   83674:	927cec45 	and	x5, x2, #0xfffffffffffffff0
   83678:	d2800003 	mov	x3, #0x0                   	// #0
   8367c:	d503201f 	nop
   83680:	3ce36820 	ldr	q0, [x1, x3]
   83684:	3ca36800 	str	q0, [x0, x3]
   83688:	91004063 	add	x3, x3, #0x10
   8368c:	eb05007f 	cmp	x3, x5
   83690:	54ffff81 	b.ne	83680 <memcpy+0x28>  // b.any
   83694:	927cec45 	and	x5, x2, #0xfffffffffffffff0
   83698:	cb050083 	sub	x3, x4, x5
   8369c:	eb05005f 	cmp	x2, x5
   836a0:	8b050024 	add	x4, x1, x5
   836a4:	8b050002 	add	x2, x0, x5
   836a8:	54000740 	b.eq	83790 <memcpy+0x138>  // b.none
   836ac:	38656821 	ldrb	w1, [x1, x5]
   836b0:	38256801 	strb	w1, [x0, x5]
   836b4:	b40006e3 	cbz	x3, 83790 <memcpy+0x138>
   836b8:	39400481 	ldrb	w1, [x4, #1]
   836bc:	f100047f 	cmp	x3, #0x1
   836c0:	39000441 	strb	w1, [x2, #1]
   836c4:	54000660 	b.eq	83790 <memcpy+0x138>  // b.none
   836c8:	39400881 	ldrb	w1, [x4, #2]
   836cc:	f100087f 	cmp	x3, #0x2
   836d0:	39000841 	strb	w1, [x2, #2]
   836d4:	540005e0 	b.eq	83790 <memcpy+0x138>  // b.none
   836d8:	39400c81 	ldrb	w1, [x4, #3]
   836dc:	f1000c7f 	cmp	x3, #0x3
   836e0:	39000c41 	strb	w1, [x2, #3]
   836e4:	54000560 	b.eq	83790 <memcpy+0x138>  // b.none
   836e8:	39401081 	ldrb	w1, [x4, #4]
   836ec:	f100107f 	cmp	x3, #0x4
   836f0:	39001041 	strb	w1, [x2, #4]
   836f4:	540004e0 	b.eq	83790 <memcpy+0x138>  // b.none
   836f8:	39401481 	ldrb	w1, [x4, #5]
   836fc:	f100147f 	cmp	x3, #0x5
   83700:	39001441 	strb	w1, [x2, #5]
   83704:	54000460 	b.eq	83790 <memcpy+0x138>  // b.none
   83708:	39401881 	ldrb	w1, [x4, #6]
   8370c:	f100187f 	cmp	x3, #0x6
   83710:	39001841 	strb	w1, [x2, #6]
   83714:	540003e0 	b.eq	83790 <memcpy+0x138>  // b.none
   83718:	39401c81 	ldrb	w1, [x4, #7]
   8371c:	f1001c7f 	cmp	x3, #0x7
   83720:	39001c41 	strb	w1, [x2, #7]
   83724:	54000360 	b.eq	83790 <memcpy+0x138>  // b.none
   83728:	39402081 	ldrb	w1, [x4, #8]
   8372c:	f100207f 	cmp	x3, #0x8
   83730:	39002041 	strb	w1, [x2, #8]
   83734:	540002e0 	b.eq	83790 <memcpy+0x138>  // b.none
   83738:	39402481 	ldrb	w1, [x4, #9]
   8373c:	f100247f 	cmp	x3, #0x9
   83740:	39002441 	strb	w1, [x2, #9]
   83744:	54000260 	b.eq	83790 <memcpy+0x138>  // b.none
   83748:	39402881 	ldrb	w1, [x4, #10]
   8374c:	f100287f 	cmp	x3, #0xa
   83750:	39002841 	strb	w1, [x2, #10]
   83754:	540001e0 	b.eq	83790 <memcpy+0x138>  // b.none
   83758:	39402c81 	ldrb	w1, [x4, #11]
   8375c:	f1002c7f 	cmp	x3, #0xb
   83760:	39002c41 	strb	w1, [x2, #11]
   83764:	54000160 	b.eq	83790 <memcpy+0x138>  // b.none
   83768:	39403081 	ldrb	w1, [x4, #12]
   8376c:	f100307f 	cmp	x3, #0xc
   83770:	39003041 	strb	w1, [x2, #12]
   83774:	540000e0 	b.eq	83790 <memcpy+0x138>  // b.none
   83778:	39403481 	ldrb	w1, [x4, #13]
   8377c:	f100347f 	cmp	x3, #0xd
   83780:	39003441 	strb	w1, [x2, #13]
   83784:	54000060 	b.eq	83790 <memcpy+0x138>  // b.none
   83788:	39403881 	ldrb	w1, [x4, #14]
   8378c:	39003841 	strb	w1, [x2, #14]
   83790:	d65f03c0 	ret
   83794:	d2800003 	mov	x3, #0x0                   	// #0
   83798:	38636824 	ldrb	w4, [x1, x3]
   8379c:	38236804 	strb	w4, [x0, x3]
   837a0:	91000463 	add	x3, x3, #0x1
   837a4:	eb03005f 	cmp	x2, x3
   837a8:	54ffff81 	b.ne	83798 <memcpy+0x140>  // b.any
   837ac:	d65f03c0 	ret

00000000000837b0 <strcpy>:
   837b0:	b4000100 	cbz	x0, 837d0 <strcpy+0x20>
   837b4:	39400022 	ldrb	w2, [x1]
   837b8:	aa0003e3 	mov	x3, x0
   837bc:	34000082 	cbz	w2, 837cc <strcpy+0x1c>
   837c0:	38001462 	strb	w2, [x3], #1
   837c4:	38401c22 	ldrb	w2, [x1, #1]!
   837c8:	35ffffc2 	cbnz	w2, 837c0 <strcpy+0x10>
   837cc:	3900007f 	strb	wzr, [x3]
   837d0:	d65f03c0 	ret
   837d4:	d503201f 	nop

00000000000837d8 <strcpy_n>:
   837d8:	b4000180 	cbz	x0, 83808 <strcpy_n+0x30>
   837dc:	b4000182 	cbz	x2, 8380c <strcpy_n+0x34>
   837e0:	8b020004 	add	x4, x0, x2
   837e4:	aa0003e2 	mov	x2, x0
   837e8:	14000004 	b	837f8 <strcpy_n+0x20>
   837ec:	38001443 	strb	w3, [x2], #1
   837f0:	eb02009f 	cmp	x4, x2
   837f4:	54000080 	b.eq	83804 <strcpy_n+0x2c>  // b.none
   837f8:	39400023 	ldrb	w3, [x1]
   837fc:	91000421 	add	x1, x1, #0x1
   83800:	35ffff63 	cbnz	w3, 837ec <strcpy_n+0x14>
   83804:	3900005f 	strb	wzr, [x2]
   83808:	d65f03c0 	ret
   8380c:	aa0003e2 	mov	x2, x0
   83810:	3900005f 	strb	wzr, [x2]
   83814:	17fffffd 	b	83808 <strcpy_n+0x30>

0000000000083818 <strlen>:
   83818:	39400001 	ldrb	w1, [x0]
   8381c:	340000e1 	cbz	w1, 83838 <strlen+0x20>
   83820:	aa0003e1 	mov	x1, x0
   83824:	d503201f 	nop
   83828:	38401c22 	ldrb	w2, [x1, #1]!
   8382c:	35ffffe2 	cbnz	w2, 83828 <strlen+0x10>
   83830:	cb000020 	sub	x0, x1, x0
   83834:	d65f03c0 	ret
   83838:	d2800000 	mov	x0, #0x0                   	// #0
   8383c:	d65f03c0 	ret

0000000000083840 <atoi>:
   83840:	aa0003e2 	mov	x2, x0
   83844:	39400000 	ldrb	w0, [x0]
   83848:	7100b41f 	cmp	w0, #0x2d
   8384c:	54000440 	b.eq	838d4 <atoi+0x94>  // b.none
   83850:	91005c46 	add	x6, x2, #0x17
   83854:	d2800000 	mov	x0, #0x0                   	// #0
   83858:	14000007 	b	83874 <atoi+0x34>
   8385c:	93407c61 	sxtw	x1, w3
   83860:	8b000800 	add	x0, x0, x0, lsl #2
   83864:	91000442 	add	x2, x2, #0x1
   83868:	eb0200df 	cmp	x6, x2
   8386c:	8b000420 	add	x0, x1, x0, lsl #1
   83870:	54000300 	b.eq	838d0 <atoi+0x90>  // b.none
   83874:	39400043 	ldrb	w3, [x2]
   83878:	5100c061 	sub	w1, w3, #0x30
   8387c:	51018464 	sub	w4, w3, #0x61
   83880:	12001c25 	and	w5, w1, #0xff
   83884:	12001c84 	and	w4, w4, #0xff
   83888:	34000243 	cbz	w3, 838d0 <atoi+0x90>
   8388c:	93407c21 	sxtw	x1, w1
   83890:	710024bf 	cmp	w5, #0x9
   83894:	54fffe69 	b.ls	83860 <atoi+0x20>  // b.plast
   83898:	5100dc61 	sub	w1, w3, #0x37
   8389c:	51010465 	sub	w5, w3, #0x41
   838a0:	12001ca5 	and	w5, w5, #0xff
   838a4:	51015c63 	sub	w3, w3, #0x57
   838a8:	93407c21 	sxtw	x1, w1
   838ac:	7100149f 	cmp	w4, #0x5
   838b0:	54fffd69 	b.ls	8385c <atoi+0x1c>  // b.plast
   838b4:	710018bf 	cmp	w5, #0x6
   838b8:	8b000800 	add	x0, x0, x0, lsl #2
   838bc:	9a9f3021 	csel	x1, x1, xzr, cc  // cc = lo, ul, last
   838c0:	91000442 	add	x2, x2, #0x1
   838c4:	eb0200df 	cmp	x6, x2
   838c8:	8b000420 	add	x0, x1, x0, lsl #1
   838cc:	54fffd41 	b.ne	83874 <atoi+0x34>  // b.any
   838d0:	d65f03c0 	ret
   838d4:	91000445 	add	x5, x2, #0x1
   838d8:	d2800000 	mov	x0, #0x0                   	// #0
   838dc:	91006042 	add	x2, x2, #0x18
   838e0:	14000007 	b	838fc <atoi+0xbc>
   838e4:	93407c81 	sxtw	x1, w4
   838e8:	8b000800 	add	x0, x0, x0, lsl #2
   838ec:	910004a5 	add	x5, x5, #0x1
   838f0:	eb05005f 	cmp	x2, x5
   838f4:	8b000420 	add	x0, x1, x0, lsl #1
   838f8:	54000300 	b.eq	83958 <atoi+0x118>  // b.none
   838fc:	394000a4 	ldrb	w4, [x5]
   83900:	5100c081 	sub	w1, w4, #0x30
   83904:	51018483 	sub	w3, w4, #0x61
   83908:	12001c27 	and	w7, w1, #0xff
   8390c:	12001c66 	and	w6, w3, #0xff
   83910:	34000244 	cbz	w4, 83958 <atoi+0x118>
   83914:	93407c21 	sxtw	x1, w1
   83918:	710024ff 	cmp	w7, #0x9
   8391c:	54fffe69 	b.ls	838e8 <atoi+0xa8>  // b.plast
   83920:	5100dc83 	sub	w3, w4, #0x37
   83924:	51010481 	sub	w1, w4, #0x41
   83928:	12001c27 	and	w7, w1, #0xff
   8392c:	51015c84 	sub	w4, w4, #0x57
   83930:	93407c61 	sxtw	x1, w3
   83934:	710014df 	cmp	w6, #0x5
   83938:	54fffd69 	b.ls	838e4 <atoi+0xa4>  // b.plast
   8393c:	710018ff 	cmp	w7, #0x6
   83940:	8b000800 	add	x0, x0, x0, lsl #2
   83944:	9a9f3021 	csel	x1, x1, xzr, cc  // cc = lo, ul, last
   83948:	910004a5 	add	x5, x5, #0x1
   8394c:	eb05005f 	cmp	x2, x5
   83950:	8b000420 	add	x0, x1, x0, lsl #1
   83954:	54fffd41 	b.ne	838fc <atoi+0xbc>  // b.any
   83958:	cb0003e0 	neg	x0, x0
   8395c:	d65f03c0 	ret

0000000000083960 <atol>:
   83960:	aa0003e3 	mov	x3, x0
   83964:	d2800000 	mov	x0, #0x0                   	// #0
   83968:	91005c66 	add	x6, x3, #0x17
   8396c:	14000007 	b	83988 <atol+0x28>
   83970:	93407c41 	sxtw	x1, w2
   83974:	8b000800 	add	x0, x0, x0, lsl #2
   83978:	91000463 	add	x3, x3, #0x1
   8397c:	eb0300df 	cmp	x6, x3
   83980:	8b000420 	add	x0, x1, x0, lsl #1
   83984:	54000300 	b.eq	839e4 <atol+0x84>  // b.none
   83988:	39400062 	ldrb	w2, [x3]
   8398c:	5100c041 	sub	w1, w2, #0x30
   83990:	51018444 	sub	w4, w2, #0x61
   83994:	12001c25 	and	w5, w1, #0xff
   83998:	12001c84 	and	w4, w4, #0xff
   8399c:	34000242 	cbz	w2, 839e4 <atol+0x84>
   839a0:	93407c21 	sxtw	x1, w1
   839a4:	710024bf 	cmp	w5, #0x9
   839a8:	54fffe69 	b.ls	83974 <atol+0x14>  // b.plast
   839ac:	5100dc41 	sub	w1, w2, #0x37
   839b0:	51010445 	sub	w5, w2, #0x41
   839b4:	12001ca5 	and	w5, w5, #0xff
   839b8:	51015c42 	sub	w2, w2, #0x57
   839bc:	93407c21 	sxtw	x1, w1
   839c0:	7100149f 	cmp	w4, #0x5
   839c4:	54fffd69 	b.ls	83970 <atol+0x10>  // b.plast
   839c8:	710018bf 	cmp	w5, #0x6
   839cc:	8b000800 	add	x0, x0, x0, lsl #2
   839d0:	9a9f3021 	csel	x1, x1, xzr, cc  // cc = lo, ul, last
   839d4:	91000463 	add	x3, x3, #0x1
   839d8:	eb0300df 	cmp	x6, x3
   839dc:	8b000420 	add	x0, x1, x0, lsl #1
   839e0:	54fffd41 	b.ne	83988 <atol+0x28>  // b.any
   839e4:	d65f03c0 	ret

00000000000839e8 <atol_n>:
   839e8:	f100405f 	cmp	x2, #0x10
   839ec:	aa0003e4 	mov	x4, x0
   839f0:	54000420 	b.eq	83a74 <atol_n+0x8c>  // b.none
   839f4:	b4000501 	cbz	x1, 83a94 <atol_n+0xac>
   839f8:	8b010087 	add	x7, x4, x1
   839fc:	d2800000 	mov	x0, #0x0                   	// #0
   83a00:	14000006 	b	83a18 <atol_n+0x30>
   83a04:	93407c61 	sxtw	x1, w3
   83a08:	91000484 	add	x4, x4, #0x1
   83a0c:	9b000440 	madd	x0, x2, x0, x1
   83a10:	eb0400ff 	cmp	x7, x4
   83a14:	540002e0 	b.eq	83a70 <atol_n+0x88>  // b.none
   83a18:	39400081 	ldrb	w1, [x4]
   83a1c:	5100c023 	sub	w3, w1, #0x30
   83a20:	51018425 	sub	w5, w1, #0x61
   83a24:	12001c66 	and	w6, w3, #0xff
   83a28:	12001ca5 	and	w5, w5, #0xff
   83a2c:	34000221 	cbz	w1, 83a70 <atol_n+0x88>
   83a30:	710024df 	cmp	w6, #0x9
   83a34:	54fffe89 	b.ls	83a04 <atol_n+0x1c>  // b.plast
   83a38:	51010426 	sub	w6, w1, #0x41
   83a3c:	5100dc23 	sub	w3, w1, #0x37
   83a40:	51015c21 	sub	w1, w1, #0x57
   83a44:	12001cc6 	and	w6, w6, #0xff
   83a48:	93407c63 	sxtw	x3, w3
   83a4c:	710014bf 	cmp	w5, #0x5
   83a50:	93407c21 	sxtw	x1, w1
   83a54:	54fffda9 	b.ls	83a08 <atol_n+0x20>  // b.plast
   83a58:	710018df 	cmp	w6, #0x6
   83a5c:	91000484 	add	x4, x4, #0x1
   83a60:	9a9f3061 	csel	x1, x3, xzr, cc  // cc = lo, ul, last
   83a64:	eb0400ff 	cmp	x7, x4
   83a68:	9b000440 	madd	x0, x2, x0, x1
   83a6c:	54fffd61 	b.ne	83a18 <atol_n+0x30>  // b.any
   83a70:	d65f03c0 	ret
   83a74:	39400000 	ldrb	w0, [x0]
   83a78:	7100c01f 	cmp	w0, #0x30
   83a7c:	54fffbc1 	b.ne	839f4 <atol_n+0xc>  // b.any
   83a80:	39400483 	ldrb	w3, [x4, #1]
   83a84:	91000880 	add	x0, x4, #0x2
   83a88:	7101e07f 	cmp	w3, #0x78
   83a8c:	9a840004 	csel	x4, x0, x4, eq  // eq = none
   83a90:	17ffffd9 	b	839f4 <atol_n+0xc>
   83a94:	d2800000 	mov	x0, #0x0                   	// #0
   83a98:	d65f03c0 	ret
   83a9c:	d503201f 	nop

0000000000083aa0 <atoi_n>:
   83aa0:	b40000a1 	cbz	x1, 83ab4 <atoi_n+0x14>
   83aa4:	39400005 	ldrb	w5, [x0]
   83aa8:	7100b4bf 	cmp	w5, #0x2d
   83aac:	54000080 	b.eq	83abc <atoi_n+0x1c>  // b.none
   83ab0:	17ffffce 	b	839e8 <atol_n>
   83ab4:	d2800000 	mov	x0, #0x0                   	// #0
   83ab8:	d65f03c0 	ret
   83abc:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   83ac0:	d1000421 	sub	x1, x1, #0x1
   83ac4:	91000400 	add	x0, x0, #0x1
   83ac8:	910003fd 	mov	x29, sp
   83acc:	97ffffc7 	bl	839e8 <atol_n>
   83ad0:	cb0003e0 	neg	x0, x0
   83ad4:	a8c17bfd 	ldp	x29, x30, [sp], #16
   83ad8:	d65f03c0 	ret
   83adc:	00000000 	.inst	0x00000000 ; undefined

0000000000083ae0 <core_timer_enable>:
   83ae0:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   83ae4:	910003fd 	mov	x29, sp
   83ae8:	d2800020 	mov	x0, #0x1                   	// #1
   83aec:	d51be220 	msr	cntp_ctl_el0, x0
   83af0:	92800000 	mov	x0, #0xffffffffffffffff    	// #-1
   83af4:	d51be200 	msr	cntp_tval_el0, x0
   83af8:	d53be001 	mrs	x1, cntfrq_el0
   83afc:	52800002 	mov	w2, #0x0                   	// #0
   83b00:	d0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   83b04:	91078000 	add	x0, x0, #0x1e0
   83b08:	97fffe7a 	bl	834f0 <log_hex>
   83b0c:	d2800800 	mov	x0, #0x40                  	// #64
   83b10:	52800041 	mov	w1, #0x2                   	// #2
   83b14:	f2a80000 	movk	x0, #0x4000, lsl #16
   83b18:	b9000001 	str	w1, [x0]
   83b1c:	a8c17bfd 	ldp	x29, x30, [sp], #16
   83b20:	d65f03c0 	ret
   83b24:	d503201f 	nop

0000000000083b28 <add_timer>:
   83b28:	1e602800 	fadd	d0, d0, d0
   83b2c:	d0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   83b30:	52800002 	mov	w2, #0x0                   	// #0
   83b34:	9107c000 	add	x0, x0, #0x1f0
   83b38:	1e780001 	fcvtzs	w1, d0
   83b3c:	93407c21 	sxtw	x1, w1
   83b40:	17fffe6c 	b	834f0 <log_hex>
   83b44:	d503201f 	nop

0000000000083b48 <print_time>:
   83b48:	d65f03c0 	ret
   83b4c:	00000000 	.inst	0x00000000 ; undefined

0000000000083b50 <svc_test>:
   83b50:	d4000001 	svc	#0x0
   83b54:	d4000001 	svc	#0x0
   83b58:	d4000001 	svc	#0x0
   83b5c:	d4000001 	svc	#0x0
   83b60:	d4000001 	svc	#0x0
   83b64:	d65f03c0 	ret

0000000000083b68 <shell>:
   83b68:	d11143ff 	sub	sp, sp, #0x450
   83b6c:	a9007bfd 	stp	x29, x30, [sp]
   83b70:	910003fd 	mov	x29, sp
   83b74:	a90153f3 	stp	x19, x20, [sp, #16]
   83b78:	d0000014 	adrp	x20, 85000 <el1_evt+0x1000>
   83b7c:	910143f3 	add	x19, sp, #0x50
   83b80:	91080294 	add	x20, x20, #0x200
   83b84:	a9025bf5 	stp	x21, x22, [sp, #32]
   83b88:	d0000016 	adrp	x22, 85000 <el1_evt+0x1000>
   83b8c:	910a02d6 	add	x22, x22, #0x280
   83b90:	a90363f7 	stp	x23, x24, [sp, #48]
   83b94:	d0000017 	adrp	x23, 85000 <el1_evt+0x1000>
   83b98:	910a22f7 	add	x23, x23, #0x288
   83b9c:	d0000015 	adrp	x21, 85000 <el1_evt+0x1000>
   83ba0:	d0000018 	adrp	x24, 85000 <el1_evt+0x1000>
   83ba4:	f90023f9 	str	x25, [sp, #64]
   83ba8:	d0000019 	adrp	x25, 85000 <el1_evt+0x1000>
   83bac:	d503201f 	nop
   83bb0:	aa1303e0 	mov	x0, x19
   83bb4:	d2807fe1 	mov	x1, #0x3ff                 	// #1023
   83bb8:	97fffd28 	bl	83058 <gets_n>
   83bbc:	aa1403e1 	mov	x1, x20
   83bc0:	aa1303e0 	mov	x0, x19
   83bc4:	97fffe81 	bl	835c8 <strcmp>
   83bc8:	35000140 	cbnz	w0, 83bf0 <shell+0x88>
   83bcc:	910822a0 	add	x0, x21, #0x208
   83bd0:	97fffd58 	bl	83130 <print>
   83bd4:	aa1303e0 	mov	x0, x19
   83bd8:	d2807fe1 	mov	x1, #0x3ff                 	// #1023
   83bdc:	97fffd1f 	bl	83058 <gets_n>
   83be0:	aa1403e1 	mov	x1, x20
   83be4:	aa1303e0 	mov	x0, x19
   83be8:	97fffe78 	bl	835c8 <strcmp>
   83bec:	34ffff00 	cbz	w0, 83bcc <shell+0x64>
   83bf0:	aa1603e1 	mov	x1, x22
   83bf4:	aa1303e0 	mov	x0, x19
   83bf8:	97fffe74 	bl	835c8 <strcmp>
   83bfc:	35000080 	cbnz	w0, 83c0c <shell+0xa4>
   83c00:	528000a0 	mov	w0, #0x5                   	// #5
   83c04:	97fffc03 	bl	82c10 <reset>
   83c08:	17ffffea 	b	83bb0 <shell+0x48>
   83c0c:	aa1703e1 	mov	x1, x23
   83c10:	aa1303e0 	mov	x0, x19
   83c14:	d2800082 	mov	x2, #0x4                   	// #4
   83c18:	97fffe7e 	bl	83610 <strcmp_n>
   83c1c:	34fffca0 	cbz	w0, 83bb0 <shell+0x48>
   83c20:	9100a301 	add	x1, x24, #0x28
   83c24:	aa1303e0 	mov	x0, x19
   83c28:	97fffe68 	bl	835c8 <strcmp>
   83c2c:	34fffc20 	cbz	w0, 83bb0 <shell+0x48>
   83c30:	910a4320 	add	x0, x25, #0x290
   83c34:	97fffd3f 	bl	83130 <print>
   83c38:	aa1303e0 	mov	x0, x19
   83c3c:	97fffcd1 	bl	82f80 <puts>
   83c40:	17ffffdc 	b	83bb0 <shell+0x48>
   83c44:	d503201f 	nop

0000000000083c48 <kernel>:
   83c48:	a9bf7bfd 	stp	x29, x30, [sp, #-16]!
   83c4c:	910003fd 	mov	x29, sp
   83c50:	97fff0ec 	bl	80000 <el2_to_el1_preserve_sp>
   83c54:	97fff0f7 	bl	80030 <set_el1_evt>
   83c58:	d0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   83c5c:	910aa000 	add	x0, x0, #0x2a8
   83c60:	97fffcc8 	bl	82f80 <puts>
   83c64:	d2820001 	mov	x1, #0x1000                	// #4096
   83c68:	d2800000 	mov	x0, #0x0                   	// #0
   83c6c:	97fff431 	bl	80d30 <reserve_mem>
   83c70:	d2a00041 	mov	x1, #0x20000               	// #131072
   83c74:	d2a000c0 	mov	x0, #0x60000               	// #393216
   83c78:	97fff42e 	bl	80d30 <reserve_mem>
   83c7c:	d0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   83c80:	d0000001 	adrp	x1, 85000 <el1_evt+0x1000>
   83c84:	f9416000 	ldr	x0, [x0, #704]
   83c88:	f9417021 	ldr	x1, [x1, #736]
   83c8c:	cb000021 	sub	x1, x1, x0
   83c90:	97fff428 	bl	80d30 <reserve_mem>
   83c94:	d2a02001 	mov	x1, #0x1000000             	// #16777216
   83c98:	d2a7e000 	mov	x0, #0x3f000000            	// #1056964608
   83c9c:	97fff425 	bl	80d30 <reserve_mem>
   83ca0:	97fff904 	bl	820b0 <init_kmalloc>
   83ca4:	97ffff8f 	bl	83ae0 <core_timer_enable>
   83ca8:	d0000000 	adrp	x0, 85000 <el1_evt+0x1000>
   83cac:	1e601000 	fmov	d0, #2.000000000000000000e+00
   83cb0:	d2800001 	mov	x1, #0x0                   	// #0
   83cb4:	f9416c00 	ldr	x0, [x0, #728]
   83cb8:	97ffff9c 	bl	83b28 <add_timer>
   83cbc:	d2820000 	mov	x0, #0x1000                	// #4096
   83cc0:	97fff7d2 	bl	81c08 <kmalloc>
   83cc4:	90000002 	adrp	x2, 83000 <puts_n+0x48>
   83cc8:	aa0003e1 	mov	x1, x0
   83ccc:	912d4040 	add	x0, x2, #0xb50
   83cd0:	d2807802 	mov	x2, #0x3c0                 	// #960
   83cd4:	97fff15a 	bl	8023c <exec_usr>
   83cd8:	97ffffa4 	bl	83b68 <shell>
	...

0000000000084000 <el1_evt>:
   84000:	17fff00f 	b	8003c <_exc_not_imp>
	...
   84080:	17ffefef 	b	8003c <_exc_not_imp>
	...
   84100:	17ffefcf 	b	8003c <_exc_not_imp>
	...
   84180:	17ffefaf 	b	8003c <_exc_not_imp>
	...
   84200:	17ffefb1 	b	800c4 <_syn_handler>
	...
   84280:	17ffefcd 	b	801b4 <_irq_handler>
	...
   84300:	17ffef4f 	b	8003c <_exc_not_imp>
	...
   84380:	17ffef2f 	b	8003c <_exc_not_imp>
	...
   84400:	17ffef31 	b	800c4 <_syn_handler>
	...
   84480:	17ffef4d 	b	801b4 <_irq_handler>
	...
   84500:	17ffeecf 	b	8003c <_exc_not_imp>
	...
   84580:	17ffeeaf 	b	8003c <_exc_not_imp>
	...
   84600:	17ffee8f 	b	8003c <_exc_not_imp>
	...
   84680:	17ffee6f 	b	8003c <_exc_not_imp>
	...
   84700:	17ffee4f 	b	8003c <_exc_not_imp>
	...
   84780:	17ffee2f 	b	8003c <_exc_not_imp>
