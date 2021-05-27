.section ".text.boot"				

.global _start						


_start:								
	mrs    x1, mpidr_el1 			
	and    x1, x1, #3				
	cbz    x1, init 				
	b 	   busy_loop				

busy_loop: 	
	wfe								
	b 	   busy_loop                

init: 	

	bl from_el2_to_el1					/* required 1-1 跳至 from_el2_to_el1，將excpetion level設為1 */
	bl set_exception_vector_table		/* required 1-3 跳至 set_exception_vector_table，設定exception發生時，查找的vector_table */

	ldr    x1, =__bss_start			
	ldr    x2, =__bss_size			

loop_clear_bss:
	cbz    x2, entry_point			
	str    xzr, [x1], #8			
	sub    x2, x2, #1				
	cbnz   x2, loop_clear_bss		

entry_point:
	ldr    x1, =_start		    	
	mov    sp, x1					
	bl	   main						
	b 	   busy_loop				


/* 

	exception level 由 EL2 變 EL1

	默認情況下，Rpi3的CPU在默認啟動後便在EL2中運行，但是我們希望內核在EL1中運行。 
	因此，您的內核需要在開始時切換到EL1

	以下是助教提供的code

	1. 設置hcr_el2暫存器(Hypervisor Configuration Register)
	這個暫存器大部分bit在reset狀態是0，只有bit 31是implementation defined
	因此設定bit 31 為1，確保在EL1 也是Aarch64的 

	2. 設置spsr_el2暫存器(Saved Program Status Register)
	用於exception發生時儲存當時CPU狀態，這邊是做初始化
	設定在AArch64下EL1h 和 D,A,I,F interrupt disabled

	參照 https://kaiiiz.github.io/notes/nctu/osdi/lab3/exception-level-switch/
	http://www.lujun.org.cn/?p=1676
	可以知道是 D,A,I,F 在9,8,7,6位，EL1h是101，所以是1111000101 = 0x3c5

	3. 設置elr_el2暫存器(Exception Link Register)
	用於exception發生時儲存處理後要返回的地址
	這邊是做初始化，設為lr

	4. 最後用eret，跳至EL1，之後就在EL1運行了

*/

from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1
		
/* 
	set vbar_el1 register to exception_vector_table address. 
	1. exception_vector_table address to x0 register
	2. x0 register value to vbar_el1 status register 
*/
set_exception_vector_table:
    adr x0, exception_vector_table
    msr vbar_el1, x0
    ret