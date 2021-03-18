.section ".text.boot"
# Do not use x0 register, that is for the address for dtd
_start:
    adr x5, _start                  /* Move the start address to x5, use adr to make sure that the code would work with different address */
    mov x1, xzr
    movk x1, #7, lsl #16            /* Move 0x70000 to x1 */
    mov x2, x1                      /* Move 0x70000 to x2 */
    adr x4, _end                    /* Move the end address of the bootloader to x4 */
    cmp x5, x1                      /* Check if the code has been relocated */
    beq after_move_to_0x70000
before_move_to_0x70000:
    cmp x4, x5                      /* Self relocation */
    bls branch_to_0x70000
    ldr x3, [x5], #8
    str x3, [x1], #8
    b before_move_to_0x70000
branch_to_0x70000:
    br x2
after_move_to_0x70000:
    mov sp, x5                      /* Initialize the stack pointer */
    adr x1, __bss_start__
    adr x2, __bss_end__
set_bss_zero_loop:
    cmp x2, x1
    bls entry
    ldr xzr, [x1], #8
    b set_bss_zero_loop
entry:
    bl main
loop:
    wfe
    b loop
