#pragma once

#include <stdint.h>

#include "thread.h"

#define SYS_UART_READ 0
#define SYS_UART_WRITE 1
#define SYS_GETPID 2
#define SYS_FORK 3
#define SYS_EXEC 4
#define SYS_EXIT 5
#define SYS_OPEN 6
#define SYS_CLOSE 7
#define SYS_WRITE 8
#define SYS_READ 9
#define SYS_LIST 10

void syscall_handler(uint32_t syscall_number, trap_frame_t *trap_frame);
void sys_uart_read(trap_frame_t *trap_frame);
void sys_uart_write(trap_frame_t *trap_frame);
void sys_getpid(trap_frame_t *trap_frame);
void sys_fork(trap_frame_t *trap_frame);
void sys_exec(trap_frame_t *trap_frame);
void sys_exit();
void sys_open(trap_frame_t *trap_frame);
void sys_close(trap_frame_t *trap_frame);
void sys_write(trap_frame_t *trap_frame);
void sys_read(trap_frame_t *trap_frame);
void sys_list(trap_frame_t *trap_frame);
