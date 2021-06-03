.section ".text"

#define SYS_GETPID  1
#define SYS_READ  2
#define SYS_WRITE  3
#define SYS_EXEC  4
#define SYS_EXIT  5
#define SYS_TIME  6
#define SYS_FORK  7

.global getpid
getpid:
    mov x6, #SYS_GETPID
    svc 0
    ret

.global uart_read
uart_read:
    mov x6, #SYS_READ
    svc 0
    ret

.global uart_write
uart_write:
    mov x6, #SYS_WRITE
    svc 0
    ret

.global exec
exec:
    mov x6, #SYS_EXEC
    svc 0
    ret

.global exit
exit:
    mov x6, #SYS_EXIT
    svc 0
    ret

.global time
time:
    mov x6, #SYS_TIME
    svc 0
    ret

.global fork
fork:
    mov x6, #SYS_FORK
    svc 0
    ret