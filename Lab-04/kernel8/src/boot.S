.section ".text.boot"

__start:
	ldr     	x1, =0x31000000
    str     	x0, [x1]            // store dt loading address to 0x31000000
//cpu init
	mrs x1, mpidr_el1//read cpu id
	and x1, x1, #3
	cbz x1, master
	
proc_hang:
    wfe
	b 		    proc_hang

master:

//exception level init
	bl from_el2_to_el1

//exception table init
	bl set_exception_vector_table
	

	ldr			x0, =__bss_begin
	ldr			x1, =__bss_end
	sub			x1, x1, x0
	ldr    		x1, =__prog_end
	mov    		sp, x1
//jump to kernel_main	
	bl			kernel_main
//for failsafe
	b 			proc_hang		

//el2->el1
from_el2_to_el1:
	mov x0, (1 << 31) // EL1 uses aarch64
	msr hcr_el2, x0
	mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
	msr spsr_el2, x0
	msr elr_el2, x30
	eret // return to EL1
	


