.section ".text"

.global sys_schedule
sys_schedule:
    svc 0

.global sys_uart_read
sys_uart_read:
    svc 1

.global sys_uart_write
sys_uart_write:
    svc 2

.global sys_exec
sys_exec:
    // x0 is name, x1 is argv and will be saved into kernel stack
    svc 3

.global sys_getpid
sys_getpid:
    svc 4

.global sys_fork
sys_fork:
    svc 5

.global sys_exit
sys_exit:
    svc 6

.global sys_open
sys_open:
    svc 7

.global sys_read
sys_read:
    svc 8

.global sys_close
sys_close:
    svc 9

.global sys_write
sys_write:
    svc 10