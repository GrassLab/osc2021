
// required 1-3 set exception_vector_table

.align 11 // vector table should be aligned to 0x800 = 4096 = 2^11
.global exception_vector_table
exception_vector_table:

	// Exception from current EL while using SP_EL0

    b exception_no_handler		// branch to a handler function.
    .align 7					// entry size is 0x80, .align will pad 0, 0x80 = 2^7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
	
	// Exception from current EL while using SP_ELX (ex. EL1 -> EL1)
	// only handler Sync & irq

    b currentEL_sync_handler
    .align 7
    b currentEL_irq_handler
    .align 7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
	
	// Exception from a lower and at least one lower EL is AArch64 (ex. EL0 -> EL1)
	// only handler Sync & irq

    b lowerEL_sync_handler
    .align 7
    b lowerEL_irq_handler
    .align 7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
	
	// Exception from a lower and all lower ELs are AArch32

    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7
    b exception_no_handler
    .align 7

// required 1-4 Save the user program’s context before executing the exception handler.

// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp, 16 * 0]
    stp x2, x3, [sp, 16 * 1]
    stp x4, x5, [sp, 16 * 2]
    stp x6, x7, [sp, 16 * 3]
    stp x8, x9, [sp, 16 * 4]
    stp x10, x11, [sp, 16 * 5]
    stp x12, x13, [sp, 16 * 6]
    stp x14, x15, [sp, 16 * 7]
    stp x16, x17, [sp, 16 * 8]
    stp x18, x19, [sp, 16 * 9]
    stp x20, x21, [sp, 16 * 10]
    stp x22, x23, [sp, 16 * 11]
    stp x24, x25, [sp, 16 * 12]
    stp x26, x27, [sp, 16 * 13]
    stp x28, x29, [sp, 16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp, 16 * 0]
    ldp x2, x3, [sp, 16 * 1]
    ldp x4, x5, [sp, 16 * 2]
    ldp x6, x7, [sp, 16 * 3]
    ldp x8, x9, [sp, 16 * 4]
    ldp x10, x11, [sp, 16 * 5]
    ldp x12, x13, [sp, 16 * 6]
    ldp x14, x15, [sp, 16 * 7]
    ldp x16, x17, [sp, 16 * 8]
    ldp x18, x19, [sp, 16 * 9]
    ldp x20, x21, [sp, 16 * 10]
    ldp x22, x23, [sp, 16 * 11]
    ldp x24, x25, [sp, 16 * 12]
    ldp x26, x27, [sp, 16 * 13]
    ldp x28, x29, [sp, 16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

/*
	參考thread的context，包含
	unsigned long x19;  (8*0)
	unsigned long x20;  (8*1)
	unsigned long x21;  (8*2)
	unsigned long x22;  (8*3)
	unsigned long x23;  (8*4)
	unsigned long x24;  (8*5)
	unsigned long x25;  (8*6)
	unsigned long x26;  (8*7)
	unsigned long x27;  (8*8)
	unsigned long x28;  (8*9)
	unsigned long fp;   (8*10)
	unsigned long lr;   (8*11)
	unsigned long sp;   (8*12)
	
	// for user program (spsr_el1, elr_el1, sp_el0)(user register x0-x30)
	unsigned long spsr_el1;  (8*13)
	unsigned long elr_el1;   (8*14)
	unsigned long sp_el0;    (8*15)
	unsigned long reg[31];  // user register x0-x30 (8*16)-(8*46)

	儲存task
	也就是將user register x0-x30還有spsr_el1, elr_el1, sp_el0等狀態都存到目前的thread的context內

	步驟有點多，可以略過
	1. str x0, [sp, -8] 先將x0內容存放在sp-8的位置
	2. mrs x0, tpidr_el1 然後x0放目前的thread
	3. stp x2, x3, [x0, 8 * 18] 將x2, x3的內容存到x0偏移8*18的位置，參考上面的context
	   reg[2]是在8*18，reg[3]是8*19，其餘以此類推依序將值塞入對應位置
	4. str x30, [x0, 8 * 46]，將x30的內容存到x0偏移8*46的位置
	5. mov x9, x0，x0的內容給x9
	6. ldr x0, [sp, -8]  將sp-8位置的內容給x0
	7. stp x0, x1, [x9 ,8 * 16] 剛剛的x0跟x1，現在才放
	8. mrs x10, spsr_el1，將spsr_el1的內容給x10
	9. mrs x11, elr_el1，將elr_el1給x11
	10.mrs x12, sp_el0，將sp_el0給x12
	11.str x10, [x9, 8 * 13]，將將x10的內容存到x9偏移8*13的位置 (就是剛剛的x0)
	12.stp x11, x12, [x9, 8 * 14]，將將x11 x12的內容存到x9偏移8*14的位置 (就是剛剛的x0)

*/

// save general registers to stack
.macro save_task
	str x0, [sp, -8]
	mrs x0, tpidr_el1
	//store x0&x1 later
	stp x2, x3, [x0, 8 * 18]
	stp x4, x5, [x0, 8 * 20]
	stp x6, x7, [x0, 8 * 22]
	stp x8, x9, [x0, 8 * 24]
	stp x10, x11, [x0, 8 * 26]
	stp x12, x13, [x0, 8 * 28]
	stp x14, x15, [x0, 8 * 30]
	stp x16, x17, [x0, 8 * 32]
	stp x18, x19, [x0, 8 * 34]
	stp x20, x21, [x0, 8 * 36]
	stp x22, x23, [x0, 8 * 38]
	stp x24, x25, [x0, 8 * 40]
	stp x26, x27, [x0, 8 * 42]
	stp x28, x29, [x0, 8 * 44]
	str x30, [x0, 8 * 46]
	mov x9, x0
	ldr x0, [sp, -8]
	stp x0, x1, [x9 ,8 * 16]
	mrs x10, spsr_el1
	mrs x11, elr_el1
	mrs x12, sp_el0
	str x10, [x9, 8 * 13]
	stp x11, x12, [x9, 8 * 14]
.endm

/*
	參考thread的context，包含
	unsigned long x19;  (8*0)
	unsigned long x20;  (8*1)
	unsigned long x21;  (8*2)
	unsigned long x22;  (8*3)
	unsigned long x23;  (8*4)
	unsigned long x24;  (8*5)
	unsigned long x25;  (8*6)
	unsigned long x26;  (8*7)
	unsigned long x27;  (8*8)
	unsigned long x28;  (8*9)
	unsigned long fp;   (8*10)
	unsigned long lr;   (8*11)
	unsigned long sp;   (8*12)
	
	// for user program (spsr_el1, elr_el1, sp_el0)(user register x0-x30)
	unsigned long spsr_el1;  (8*13)
	unsigned long elr_el1;   (8*14)
	unsigned long sp_el0;    (8*15)
	unsigned long reg[31];  // user register x0-x30 (8*16)-(8*46)

	還原task
	也就是將由目前的thread的context還原至user register x0-x30還有spsr_el1, elr_el1, sp_el0暫存器

	步驟有點多，可以略過
	1. mrs x9, tpidr_el1 先將x9放目前的thread
	2. ldr x10, [x9, 8 * 13] 將x9偏移8*13位置的內容給x10
	3. ldp x11, x12, [x9, 8 * 14] 將x9偏移8*14 8*15位置的內容給x11跟x12
	4. msr spsr_el1, x10 ，x10的內容給spsr_el1
	5. msr elr_el1, x11，x11的內容給elr_el1
	6. msr sp_el0, x12，x12的內容給sp_el0
	7. mov x0, x9， 把x9的內容給x0 (就是目前thread)
	8. ldp x2, x3, [x0, 8 * 18]，將x0偏移8*18位置的內容給x2, x3
	   後面以此類推，依序將值塞入對應位置

*/

// load general registers from stack
.macro restore_task
	mrs x9, tpidr_el1
	ldr x10, [x9, 8 * 13]
	ldp x11, x12, [x9, 8 * 14]
	msr spsr_el1, x10
	msr elr_el1, x11
	msr sp_el0, x12
	mov x0, x9
	//restore x0&x1 later
	ldp x2, x3, [x0, 8 * 18]
	ldp x4, x5, [x0, 8 * 20]
	ldp x6, x7, [x0, 8 * 22]
	ldp x8, x9, [x0, 8 * 24]
	ldp x10, x11, [x0, 8 * 26]
	ldp x12, x13, [x0, 8 * 28]
	ldp x14, x15, [x0, 8 * 30]
	ldp x16, x17, [x0, 8 * 32]
	ldp x18, x19, [x0, 8 * 34]
	ldp x20, x21, [x0, 8 * 36]
	ldp x22, x23, [x0, 8 * 38]
	ldp x24, x25, [x0, 8 * 40]
	ldp x26, x27, [x0, 8 * 42]
	ldp x28, x29, [x0, 8 * 44]
	ldr x30, [x0, 8 * 46]
	ldp x0, x1, [x0, 8 * 16]
.endm

lowerEL_sync_handler:
    save_task
    bl lowerEL_sync_interrupt	// exception.c
    restore_task
    eret

currentEL_sync_handler:
    save_all
    bl exception_entry	// exception.c
    load_all
    eret

lowerEL_irq_handler:
    save_all
    bl lowerEL_irq_interrupt	// exception.c
    load_all
    eret

currentEL_irq_handler:
    save_all
    bl currentEL_irq_interrupt	// exception.c
    load_all
    eret
	
exception_no_handler:
    save_all
    bl no_exception_handle	// exception.c
    load_all
    eret