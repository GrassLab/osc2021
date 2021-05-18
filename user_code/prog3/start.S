.section ".text"
.global _start

_start:
    bl main
    bl exit
_hang:
    b _hang
