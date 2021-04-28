.section ".text.boot"				// Declare a section of .text.boot

.global _start						/* The .global keyword is used to make a symbol visible to the linker
									  .global _start makes the _start symbol to a visible global symbol */

// The bootloader of rpi3 loads kernel8.img to RAM (address 0x80000) by GPU
// After that, 4 CPU cores start to run the same code simultaneously.
// So, let only one core proceed, and let others enter a busy loop.

_start:								// _start represents the program start entry symbol, the following is its execution content 
	mrs    x1, mpidr_el1 			// gets the processor ID from the 'mpidr_el1' system register to x1
	and    x1, x1, #3				// and bit, because rpi3 have 4 CPU cores, so and #3
	cbz    x1, init 				// compare, if cpu id = 0 jump to init. 
	b 	   busy_loop				// else jump to busy_loop

busy_loop: 	
	wfe								// Let ARM enter the 'low-power standby state' amd wait for event
	b 	   busy_loop                // b means jump, the this sentence means jump back to busy_loop

init:
    ldr x1, =0x9000000
    str x0, [x1]
 	
	ldr    x1, =__bss_start			// init bss segment, bss segment are initialized to 0, .bss start address to x1
	ldr    x2, =__bss_size			// size of the .bss section to x2

loop_clear_bss:
	cbz    x2, entry_point			// if .bss size = 0, jump to entry_point
	str    xzr, [x1], #8			// store register. xzr is zero register. *x1 = 0; x1 += 8;
	sub    x2, x2, #1				// x2 = x2 - 1; (because __bss_size / 8)
	cbnz   x2, loop_clear_bss		// if .bss size > 0 jump to loop_clear_bss. 

entry_point:
	ldr    x1, =_start		    	// sp is stack pointer, mov _start to stack top 
	mov    sp, x1					// 
	bl	   main						// jump to our main() method of main.c
	b 	   busy_loop				// should never come here, if failure jump to busy_loop

