.section ".text"

.global _start
.global delay

_start:
  b main

delay:
  subs x0, x0, #1
  bne delay
  ret
