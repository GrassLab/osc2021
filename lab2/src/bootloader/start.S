.section ".text.boot"
_start:
    adr x0, _start
    mov x1, xzr
    movk x1, #7, lsl #16
    mov x2, x1
    adr x4, _end
    cmp x0, x1
    beq after_move_to_0x70000
before_move_to_0x70000:
    cmp x4, x0
    bls branch_to_0x70000
    ldr x3, [x0], #8
    str x3, [x1], #8
    b before_move_to_0x70000
branch_to_0x70000:
    br x2
after_move_to_0x70000:
    mov sp, x0
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
