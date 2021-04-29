.section ".text.start"
.global _start
_start:
    bl      main
    b       sys_exit