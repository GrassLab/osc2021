	
.globl call_sys_gitPID
call_sys_getPID:
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
	
.globl call_sys_open
call_sys_open:
    svc #7
	mov x0, x0
    ret

.globl call_sys_close
call_sys_close:
    svc #8
	mov x0, x0
    ret

.globl call_sys_write
call_sys_write:
    svc #9
	mov x0, x0
    ret
	
.globl call_sys_read
call_sys_read:
    svc #10
	mov x0, x0
    ret