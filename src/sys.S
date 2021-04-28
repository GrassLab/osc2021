#include "sys.h"

.globl call_sys_write
call_sys_write:
    mov w8, #SYS_WRITE_NUMBER
    svc #0
    ret

.globl call_sys_uart_write
call_sys_uart_write:
    mov w8, #SYS_UARTWRITE_NUMBER
    svc #0
    ret

.globl call_sys_uart_read
call_sys_uart_read:
    mov w8, #SYS_UARTREAD_NUMBER
    svc #0
    ret

.globl call_sys_gitPID
call_sys_gitPID:
    mov w8, #SYS_GETPID_NUMBER
    svc #0
    ret

.globl call_sys_fork
call_sys_fork:
    mov w8, #SYS_FORK_NUMBER
    svc #0
    ret

.globl call_sys_exec
call_sys_exec:
    mov w8, #SYS_EXEC_NUMBER
    svc #0
    ret

.globl call_sys_exit
call_sys_exit:
    mov w8, #SYS_EXIT_NUMBER
    svc #0
    ret

.globl call_sys_malloc
call_sys_malloc:
    mov w8, #SYS_MALLOC_NUMBER
    svc #0
    ret

.globl call_sys_coreTimer_on
call_sys_coreTimer_on:
    mov w8, #SYS_CORETIMER_On
    svc #0
    ret

.globl call_sys_coreTimer_off
call_sys_coreTimer_off:
    mov w8, #SYS_CORETIMER_OFF
    svc #0
    ret

