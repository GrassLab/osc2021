.section ".text.boot"

.global _start

_start:
    bl     main
    svc    #10
    ret
