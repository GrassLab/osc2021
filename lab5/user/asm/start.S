.section ".text.boot"

.global _start_main
_start_main:
  bl main
  b exit