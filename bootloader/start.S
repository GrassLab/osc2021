.section = ".text"

.global _start

_start:
    mrs x0, mpidr_el1 //get CPU id
    and x0, x0, #3 
    cbz x0, 2f //if CPU id == 0, goto 2

//infinite loop for CPU id != 0
1:
    wfe
    b 1b

2:
    ldr x0, = bss_begin
    ldr x1, = bss_end
    subs x1, x1, x0
    b.ls init_register_move //if bss size = 0, no need to clear bss

memzero: //clear the bss
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero

// move the section started with 0x00080000 and end with bss_end to 0x00060000
init_register_move: 
    ldr x0, = 0x00080000
    ldr x1, = bss_end
    ldr x2, = 0x00060000
    subs x1, x1, x0
    b.ls jump_to_new_address //if the size of section from 0x00080000 to bss_end is zero, no nead to move or clear this section

move_bootloader: 
    ldr x3, [x0], #8
    str x3, [x2], #8
    subs x1, x1, #8
    b.gt move_bootloader
    b jump_to_new_address

// clear the section from 0x00080000 to bss_end
init_register_clear:
    ldr x0, = 0x00080000
    ldr x1, = bss_end
    ldr x2, = main // record the previous address of main 
    subs x2, x2, #0x00080000 // calculate the offset of main function
    add x2, x2, #0x00060000 // x2 store the new start address of main function
    subs x1, x1, x0
    b.ls goto_main

clear_bootloader:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt clear_bootloader
    b goto_main

jump_to_new_address:
    ldr x0, = __stack_top
    mov sp, x0 //set stack
    ldr x0, = init_register_clear
    subs x0, x0, #0x00080000 // calculate the offset of init_register_clear
    add x0, x0, #0x00060000 // x0 store the new start address of init_register_clear
    br x0 // jump to the new start address of init_register_clear

goto_main:
    br x2 // jump to new address of main
    b 1b
