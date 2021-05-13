#include "sys.h"

.globl call_sys_print
call_sys_print:
    mov w8, #SYS_PRINT_NUMBER
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

.globl call_sys_open
call_sys_open:
    mov w8, #SYS_OPEN
    svc #0
    ret

.globl call_sys_close
call_sys_close:
    mov w8, #SYS_CLOSE
    svc #0
    ret

.globl call_sys_write
call_sys_write:
    mov w8, #SYS_WRITE
    svc #0
    ret

.globl call_sys_read
call_sys_read:
    mov w8, #SYS_READ
    svc #0
    ret

.globl call_sys_read_directory
call_sys_read_directory:
    mov w8, #SYS_READ_DIRECTORY
    svc #0
    ret

.globl call_sys_mkdir
call_sys_mkdir:
    mov w8, #SYS_MKDIR
    svc #0
    ret

.globl call_sys_chdir
call_sys_chdir:
    mov w8, #SYS_CHDIR
    svc #0
    ret

.globl call_sys_mount
call_sys_mount:
    mov w8, #SYS_MOUNT
    svc #0
    ret

.globl call_sys_unmount
call_sys_unmount:
    mov w8, #SYS_UNMOUNT
    svc #0
    ret

