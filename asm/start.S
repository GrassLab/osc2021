.section ".text.boot"

.global _start

_start:
    // read cpu id, stop slave cores
    // read the content of mpidr_el1, indicating the core number, into x1
    mrs     x1, mpidr_el1
    // keep only the first and the second bit
    and     x1, x1, #3
    // jump to label 2 if x1 is zero
    cbz     x1, 2f
    // cpu id > 0, stop
    // wfe puts the core into a low-power standby mode
1:  wfe
    // an infinity loop forms here
    b       1b
2:  // cpu id == 0

    // set stack before our code
    // ldr loads data from memory to register, which is _start here
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =_bss_start
    ldr     w2, =_bss_size
3:  cbz     w2, 4f
    # set the content x1 points to as zero
    # and x1 = x1 + 8
    str     xzr, [x1], #8
    # w2 = w2 - 1
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to C code, should not return
    // bl is to branch with the address of the next instruction saved, 
    // whereas b is to simply branch and will not return
4:  bl      main
    // for failsafe, halt this core too
    b       1b