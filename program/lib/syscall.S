.section ".text"

.macro gensys name num
.global \name
\name:
    stp fp, lr, [sp, #-16]!
    mov fp, sp
    mov x8, \num
    svc #0
    ldp fp, lr, [sp], #16
    ret
.endm

gensys schedule 0
gensys clone    1
gensys exec     2
gensys exit     3
gensys uart_put 4
gensys getpid   5
gensys uart_get 6
gensys fork     7
gensys delay    8
gensys wait     9
gensys open    10
gensys read    11
gensys write   12
gensys close   13
