.section = ".text"

.global _start

_start:
    ldr x0, = bss_begin // load bss_begin address to x0
    ldr x1, = bss_end // load bss_end address to x1
    subs x1, x1, x0 // set x1 = bss_end - bss_begin(bss size)
    b.ls move_bootloader_setting //branch to move_bootloader, if bss_end < bss_begin (unsigned)

memzero: //clear the bss
	str xzr, [x0], #8 //將xzr(zero register)中的字數據寫入以x0(bss_begin)為地址的存儲器中，並將新地址R1＋8寫入x0(x0 = x0 + 8)。
	subs x1, x1, #8  //x1 = bss_size(剩餘的) - 8, and set Condition Code Flags(N,Z,C,V)
	b.gt memzero //(greater than, signed)(judge form previous *s instruction),(when bss_size_left > 0, branch)

move_bootloader_setting:
    ldr x0, = 0x00080000
    ldr x1, = bss_end
    ldr x2, = 0x00060000
    subs x1, x1, x0 
    b.ls set_stackpointer_and_jump

move_bootloader:
    ldr x3, [x0], #8
    str x3, [x2], #8
    subs x1, x1, #8
    b.gt move_bootloader
    b set_stackpointer_and_jump

clear_bootloader_setting:
    ldr x0, = 0x00080000
    ldr x1, = bss_end
    ldr x2, = main
    sub x2, x2, #0x00080000
    add x2, x2, #0x00060000
    subs x1, x1, x0 
    b.ls 2f

clear_bootloader:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt clear_bootloader
    b 2f

set_stackpointer_and_jump:
    ldr x0, = __stack_top // load _start address to x0
    mov sp, x0 //set stack
    ldr x0, = clear_bootloader_setting
    sub x0, x0, #0x00080000
    add x0, x0, #0x00060000
    br x0 //jump to _start at 0x00060000

2:
    br x2

1:
    b 1b //branch to 1 back
