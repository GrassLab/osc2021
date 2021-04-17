.section .text
.global _start

_start:
    ldr     	x1, =0x100000
    str     	x0, [x1]            // store dt loading address to 0x1000000
    
    adr     	x0, __start
    adr     	x1, __start
    mov     	x2, x1
    adr     	x4, proc_hang
    cmp     	x0, x1
    beq     	clear_bss
    
proc_hang:
    wfe
	b 		    proc_hang
	
clear_bss:	
    adr 		x0, __bss_start
    adr 		x1, __bss_end
    sub 		x1, x1, x0
	ldr    		x1, =__stack_top
	mov    		sp, x1
  	
loop_end:
  	bl 			kernel_main
    b 			proc_hang

    
memzero:
    str 		xzr, [x0], #8
    subs 		x1, x1, #8
    b.gt 		memzero
    ret

