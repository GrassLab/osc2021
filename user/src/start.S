.section ".text.boot"

.global _start

_start:
    bl main
    b exit

.section ".text"

.global uart_read
.global uart_write
.global getpid
.global fork
.global exec
.global exit
.global open
.global close
.global write
.global read
.global list
.global mkdir
.global chdir
.global mount
.global umount

uart_read:
    svc 0
    ret

uart_write:
    svc 1
    ret

getpid:
    svc 2
    ret

fork:
    svc 3
    ret

exec:
    svc 4

exit:
    svc 5
    ret

open:
    svc 6
    ret

close:
    svc 7
    ret

write:
    svc 8
    ret

read:
    svc 9
    ret

list:
    svc 10
    ret

mkdir:
    svc 11
    ret

chdir:
    svc 12
    ret

mount:
    svc 13
    ret

umount:
    svc 14
    ret