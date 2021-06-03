	
.globl call_sys_gitPID
call_sys_gitPID:
    svc #1
	mov x0, x0
    ret

.globl call_sys_uart_read
call_sys_uart_read:
    svc #2
	mov x0, x0
    ret

.globl call_sys_uart_write
call_sys_uart_write:
    svc #3
	mov x0, x0
    ret
	
.globl call_sys_exec
call_sys_exec:
    svc #4
	mov x0, x0
    ret

.globl call_sys_exit
call_sys_exit:
    svc #5
    ret

.globl call_sys_fork
call_sys_fork:
    svc #6
	mov x0, x0
    ret
