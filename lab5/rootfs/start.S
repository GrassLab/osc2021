.section ".text.boot"

.global _start

_start:
1:
    bl main
    b exit
