.section = ".text"

.global _start

_start:
//cpu init
	mrs x1, mpidr_el1//read cpu id
	and x1, x1, #3
	cbz x1, 2f
1://id>0, stop
	wfe
	b 1b
2://id=0, exec
    bl from_el2_to_el1
    bl set_exception_vector_table

#     ldr x0, = bss_begin // load bss_begin address to x0
#     ldr x1, = bss_end // load bss_end address to x1
#     subs x1, x1, x0 // set x1 = bss_end - bss_begin(bss size)
#     b.ls 3f //branch to 2, if bss_end < bss_begin (unsigned)

# memzero: //clear the bss
# 	str xzr, [x0], #8 //將xzr(zero register)中的字數據寫入以x0(bss_begin)為地址的存儲器中，並將新地址R1＋8寫入x0(x0 = x0 + 8)。
# 	subs x1, x1, #8  //x1 = bss_size(剩餘的) - 8, and set Condition Code Flags(N,Z,C,V)
# 	b.gt memzero //(greater than, signed)(judge form previous *s instruction),(when bss_size_left > 0, branch)

3:
    ldr x0, = __stack_top // load _start address to x0
    mov sp, x0 //set stack
    bl main //jump to main.c

1:
    b 1b //branch to 1 back
