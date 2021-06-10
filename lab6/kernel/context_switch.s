/*
	switch_to (prev, next), x0:prev thread, x1:next thread
	1. 將 x19-x28, fp, lr, sp 暫存器內容儲存到 prev thread
	2. 將 next thread的內容給 x19-x28, fp, lr, sp 暫存器
	3. 設定 x1(next) 為目前執行的thread 
	
	x29	FP	The Frame Pointer.  fp儲存stack的底端
	x30	LR	The Link Register.  lr儲存function的連結位址
		SP	The Stack Pointer.  sp儲存stack的頂端
		
	stp x19, x20, [x0, 16 * 0] // 將x19, x20的內容存到x0偏移16*0的位置
	mov x9, sp                 // 將sp內容給x9
	str x9, [x0, 16 * 6]       // 將x9的內容存到x0偏移16*6的位置
	
	ldp x19, x20, [x1, 16 * 0] // 將x1偏移16*0位置的內容給x19, x20
	ldr x9, [x1, 16 * 6]       // 將x1偏移16*6位置的內容給x9
	mov sp,  x9                // 將x9內容給sp
*/

.global switch_to
switch_to:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp fp, lr, [x1, 16 * 5]
    ldr x9, [x1, 16 * 6]
    mov sp,  x9
    msr tpidr_el1, x1
    ret

/*
	Get the current thread from tpidr_el1 register
*/

.global get_current
get_current:
    mrs x0, tpidr_el1
    ret

/*
	set the current thread (tpidr_el1 register)
*/
	
.global set_current
set_current:
    msr tpidr_el1, x0
    ret