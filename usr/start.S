.section ".text.boot"

.global _start

_start:
  ldp x0, x1, [sp]
  bl main
  b exit

.section ".text"

.global read
.global write
.global get_pid
.global fork
.global execve
.global exit

read:
  svc 0
  ret

write:
  svc 1
  ret

get_pid:
  svc 39
  ret

fork:
  svc 57
  ret

execve:
  svc 59

exit:
  svc 60
  ret