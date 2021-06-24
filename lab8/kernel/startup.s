.section ".text.boot"	

/*
	lab8 requirement 1-1 Set up TCR_EL1
	Translation Control Register (TCR) 不同層級Translation table轉換的控制暫存器(EL0-EL1)
	https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/TCR-EL1--Translation-Control-Register--EL1-
	設置頁面大小=4KB
	2^12 = 4KB, 須設定第0(T0SZ)及第16 bit(T1SZ), The size offset of the memory region addressed by TTBR0_EL1. The region size is 2^(64-T0SZ) bytes.
*/

.equ TCR_CONFIG_REGION_48bit, (((64 - 48) << 0) | ((64 - 48) << 16))
.equ TCR_CONFIG_4KB, ((0b00 << 14) |  (0b10 << 30))
.equ TCR_CONFIG_DEFAULT, (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

/*
	lab8 requirement 1-2 Set up mair_el1
	Memory Attribute Indirection Register (MAIR) 
	本實驗使用的記憶體屬性有 https://grasslab.github.io/NYCU_Operating_System_Capstone/labs/lab8.html#page-attr
	G/nG: 多個memory訪問可以/無法合併, R/nR: 允許訪問指令可以/不可以重排，E/nE: 寫入操作ack必須直接目的地/可以是中間buffer 
	設定Device為nGnRnE，最受限制，以及Normal為no cache
*/

.equ MAIR_DEVICE_nGnRnE, 0b00000000
.equ MAIR_NORMAL_NOCACHE, 0b01000100
.equ MAIR_IDX_DEVICE_nGnRnE, 0
.equ MAIR_IDX_NORMAL_NOCACHE, 1

/*
	lab8 requirement 1-3 Set up identity paging.
	lab8 requirement 1-5 Linear map kernel with finer granularity and map RAM as normal memory.
	
	Entry of PGD, PUD, PMD which point to a page table
	+----+----------------------------+---------+----+
    |    |                            | igonred | 11 |
	+----+----------------------------+---------+----+
	    47                            12        2    0
	
	Entry of PUD, PMD which point to a block
	+----+----------------------------+---------+----+
    |    |                            |attribute| 01 |
	+----+----------------------------+---------+----+
	    47                            12        2    0
		
	Entry of PTE which point to a page
	+----+----------------------------+---------+----+
    |    |                            |attribute| 11 |
	+----+----------------------------+---------+----+
	    47                            12        2    0
	
	所以PD_TABLE = 0b11,  PD_BLOCK = 0b01.  PD_PAGE = 0b11
	
	● PGD: 因為PGD前面為igonred，所以 BOOT_PGD_ATTR, PD_TABLE
	● PUD: 前面為memory attribute
	● PTE: 前面為memory attribute
	
	Bits[10]
    The access flag, a page fault is generated if not set.
	
	因此PD_ACCESS 設為 (1 << 10)
	
	依1-3需求說明，PUD為一個block且 Map all memory as Device nGnRnE，因此PUD的momery attribute設定為
	BOOT_PUD_ATTR, (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
	
	後續1-5需求說明修正
	更細分為PMD, PTE，PTE分為DEVICE_nGnRnE，以及Normal的
*/

.equ PERIPHERAL_BASE, 0x3f000000
.equ PAGE_TABLE_BASE, 0x30000000
.equ PGD_BASE, (PAGE_TABLE_BASE + 0x0000)
.equ PUD_BASE, (PAGE_TABLE_BASE + 0x1000)
.equ PMD_BASE, (PAGE_TABLE_BASE + 0x2000)
.equ PTE_BASE, (PAGE_TABLE_BASE + 0x4000)

.equ PD_TABLE, 0b11
.equ PD_BLOCK, 0b01
.equ PD_PAGE, 0b11
.equ PD_ACCESS, (1 << 10)
.equ BOOT_PGD_ATTR, PD_TABLE
.equ BOOT_PUD_ATTR, PD_TABLE
.equ BOOT_PMD_ATTR, PD_TABLE
.equ BOOT_PTE_DEVICE_nGnRnE_ATTR, (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_PAGE)
.equ BOOT_PTE_NORMAL_NOCACHE_ATTR, (PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2) | PD_PAGE)

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
	
	bl init_mmu  /* lab8需求, 初始化mmu */

boot_rest:
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
	
	// IMPORTANT: disable exceptions of accessing the SIMD and floating-point registers
	mov x0, #(3 << 20)
	msr cpacr_el1, x0
	
    eret // return to EL1
		
/* 
	set vbar_el1 register to exception_vector_table address. 
	1. exception_vector_table address to x0 register
	2. x0 register value to vbar_el1 status register 
*/
set_exception_vector_table:
    ldr x0, =exception_vector_table
    msr vbar_el1, x0
    ret
	
init_mmu:

	/* lab8 requirement 1-1 Set up TCR_EL1 */
	
	ldr x0, =TCR_CONFIG_DEFAULT
	msr tcr_el1, x0
	
	/* lab8 requirement 1-2 Set up mair_el1 */
	
	ldr x0, =((MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) | (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8)))
	msr mair_el1, x0
	
	/* 
	    lab8 requirement 1-3 Set up identity paging. 
		lab8 requirement 1-4 Modify the linker script, and map the kernel space.
		lab8 requirement 1-5 Linear map kernel with finer granularity and map RAM as normal memory.
	*/
	
    ldr x0, =PGD_BASE // PGD's page frame at 0x0
    ldr x1, =PUD_BASE // PGD's page frame at 0x1000
    ldr x2, =PMD_BASE // PGD's page frame at 0x2000
    ldr x3, =PTE_BASE // PGD's page frame at 0x4000
	
	/* step1: 設定PGD，為一個PD table，physical address指向PUD */
    ldr x4, =BOOT_PGD_ATTR
    mov x5, x1
    orr x6, x5, x4 // combine the physical address of next level page with attribute.
    str x6, [x0]

	/* step2: 設定 PUD，分兩塊，每塊1GB，physical address指向PMD */
    ldr x4, =BOOT_PUD_ATTR
    mov x5, x2
    orr x6, x5, x4
    str x6, [x1] // 1st 1GB mapped by the 1st entry of PUD
    add x5, x5, #0x1000
    orr x6, x5, x4
    str x6, [x1, 8] // 2nd 1GB mapped by the 2nd entry of PUD

	/* step3: 設定PMD，分成1024塊，每塊2MB，physical address指向PTE */
    ldr x4, =BOOT_PMD_ATTR
    mov x5, x3
    mov x7, x2
    mov x9, #(512 * 2)
set_PMD:
    orr x6, x5, x4
    str x6, [x7] // 2MB block
    sub x9, x9, #1
    add x7, x7, #8
    add x5, x5, #0x1000
    cbnz x9, set_PMD

	/* setp4: 設定PTE，分成512*512*2塊，每塊4KB，大部分 RAM 映射為普通記憶體，將 MMIO 區域映射為裝置記憶體 */
    ldr x4, =BOOT_PTE_NORMAL_NOCACHE_ATTR
    mov x5, #0x00000000
    mov x7, x3
    mov x9, #(512 * 512 * 2)
    ldr x10, =PERIPHERAL_BASE
set_PTE:
    cmp x5, x10
    blt normal_mem
    ldr x4, =BOOT_PTE_DEVICE_nGnRnE_ATTR
normal_mem:
    orr x6, x5, x4
    str x6, [x7] // 4KB page
    sub x9, x9, #1
    add x7, x7, #8
    add x5, x5, #(1 << 12)
    cbnz x9, set_PTE

	/* setp5: 設定ttbr0_el1, ttbr1_el1 */
	msr ttbr0_el1, x0 // load PGD to the bottom translation based register.
	msr ttbr1_el1, x0 // also load PGD to the upper translation based register.

	/* setp6: 啟動MMU */
    mrs x2, sctlr_el1
    orr x2 , x2, 1
    msr sctlr_el1, x2 // enable MMU, cache remains disabled
		
	/* setp7: 跳轉到boot_rest */	
	ldr x2, =boot_rest // indirect branch to the virtual address
    br x2