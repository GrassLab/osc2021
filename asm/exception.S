/*
exception_vector_table:
    
    // Current EL with SP0
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError
    
    // Current EL with SPx
    .align 7
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError
    
    // Lower EL using AArch64
    .align 7
    b sync_handler          // Synchronous
    .align 7
    b irq_handler           // IRQ
    .align 7 
    b exception_handler     // FIQ
    .align 7 
    b exception_handler     // SError

    // Lower EL using AArch32
    .align 7
    b exception_handler     // Synchronous
    .align 7
    b exception_handler     // IRQ
    .align 7
    b exception_handler     // FIQ
    .align 7
    b exception_handler     // SError
*/