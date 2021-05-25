.section ".text.boot"

.global _start

_start:
    bl main
    b exit
