.global _start
_start:
	mv sp,#0x80000
	bl main
	b .
