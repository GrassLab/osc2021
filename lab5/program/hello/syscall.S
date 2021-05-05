.section ".text"

.macro gensys name num
.global \name
\name:
    mov x8, \num
    svc #0
    ret
.endm

gensys schedule 0
gensys clone 1
gensys exec 2
gensys exit 3
gensys uart_put 4
gensys uart_write 5
gensys getpid 5
