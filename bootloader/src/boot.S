.section .text
.global _start

_start:
    ldr     	x1, =0x90000
    str     	x0, [x1]            // store dt loading address to 0x1000000
    
    adr     	x0, __start
    ldr     	x1, =0x80000
    mov     	x2, x1
    adr     	x4, proc_hang
    cmp     	x0, x1
    beq     	clear_bss


	
clear_bss:	
    adr 		x0, __bss_start
    adr 		x1, __bss_end
    sub 		x1, x1, x0
	ldr    		x1, =__stack_top
	mov    		sp, x1
  	
loop_end:
  	bl 			bootloader
    b 			proc_hang

    
memzero:
    str 		xzr, [x0], #8
    subs 		x1, x1, #8
    b.gt 		memzero
    ret
    
proc_hang:
    wfe
	b 		    proc_hang