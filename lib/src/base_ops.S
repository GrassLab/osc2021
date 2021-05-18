.globl put32
put32:
    str     w1, [x0]
    ret

.globl get32
get32:
    ldr     w0, [x0]
    ret

.globl wait
wait:
	subs    x0, x0, #1
	bne     wait
	ret

.global clear_bss
clear_bss:
      // clear bss memory
    str     xzr, [x0], #8
    subs    x1, x1, #8
    bgt     clear_bss
    ret 