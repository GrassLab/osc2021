#define SYS_GETPID 1
#define SYS_UART_READ 2
#define SYS_UART_WRITE 3
#define SYS_EXEC 4
#define SYS_EXIT 5


// Program Runtime
.section ".text._runtime"
.global _runtime_start
_runtime_start:
    // jump to main function
    bl main

.global _runtime_end
_runtime_end:
    // should called exit to notify the system
    bl exit
    // the exit would never return
    b _runtime_end

.section ".text._syscalls"
.global getpid
getpid:
    mov x8, SYS_GETPID
    svc 0
    ret

.global uart_write
uart_write:
    mov x8, SYS_UART_WRITE
    svc 0
    ret

.global uart_read
uart_read:
    mov x8, SYS_UART_READ
    svc 0
    ret

.global exec
exec:
    mov x8, SYS_EXEC
    svc 0
    ret

.global exit
exit:
    mov x8, SYS_EXIT
    svc 0
    ret