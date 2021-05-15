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

uart_read:
    svc 0
    ret

uart_write:
    svc 1
    ret

getpid:
    svc 39
    ret

fork:
    svc 57
    ret

exec:
    svc 59

exit:
    svc 60
    ret
