#define SYS_GETPID 1
#define SYS_UART_READ 2
#define SYS_UART_WRITE 3
#define SYS_EXEC 4
#define SYS_EXIT 5

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