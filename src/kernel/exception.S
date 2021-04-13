.global exec_in_el0
.global exception_vector_table
.global set_exception_vector_table

.align 11 // vector table should be aligned to 0x800
exception_vector_table:
    b exception_handler // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7
    b exception_handler
    .align 7

set_exception_vector_table:
    adr   x0, exception_vector_table
    msr   vbar_el1, x0
    ret   lr

exception_handler:
    mrs   x0, spsr_el1
    mrs   x1, elr_el1
    mrs   x2, esr_el1
    bl    print_num

exec_in_el0:
    msr   elr_el1, x0
    mov   x1, 0x3c0
    msr   spsr_el1, x1 

    // set sp of el0
    add   x2, x0, 0x10000
    msr   sp_el0, x2 
    
    eret