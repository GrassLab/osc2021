.section ".text.boot"

.global _start

_start:
  ldp x0, x1, [sp]
  bl main
  b exit

.section ".text"

.global read
.global write
.global open
.global close
.global get_pid
.global fork
.global execve
.global exit
.global mkdir
.global rmdir
.global create
.global remove
.global chdir
.global mount
.global umount
.global getdent

read:
  svc 0
  ret

write:
  svc 1
  ret

open:
  svc 2
  ret

close:
  svc 3
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

mkdir:
  svc 83
  ret

rmdir:
  svc 84
  ret

create:
  svc 85
  ret

remove:
  svc 86
  ret

chdir:
  svc 87
  ret

mount:
  svc 88
  ret

umount:
  svc 89
  ret

getdent:
  svc 90
  ret