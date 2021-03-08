.section ".text.boot"

_start:
//cpu init
	mrs x1, mpidr_el1//read cpu id
	and x1, x1, #3
	cbz x1, 2f
1://id>0, stop
	wfe
	b 1b
2://id=0, exec

//bss init
	ldr x1, =__bss_start//64-bit load
	ldr w2, =__bss_size//32-bit load
3:
	cbz w2, 4f
	str xzr, [x1], #8//64-bit store
	sub w2, w2, #1
	b 3b
4:

//stack init
	ldr x1, =_start
	mov sp, x1

//jump to main
	bl main//function call

//for failsafe
	b 1b