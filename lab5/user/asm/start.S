.section ".text.boot"

.global _main

_start_main:
  bl main
  b exit