.section ".text.boot"

.global _start

_start:
  bl main
  svc 60
