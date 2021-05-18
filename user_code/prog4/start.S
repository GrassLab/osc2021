.section ".text"
.global _start

_start:
    bl main
_hang:
    bl exit
    b _hang
