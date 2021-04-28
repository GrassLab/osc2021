#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <stddef.h>
#include "thread.h"
#include "miniuart.h"
#include "cpio.h"

void sys_getpid(uint64_t *trap_frame);
void sys_uart_read(uint64_t *trap_frame);
void sys_uart_write(uint64_t *trap_frame);
void sys_exec(uint64_t *trap_frame);
void sys_exit(uint64_t *trap_frame);
void sys_fork(uint64_t *parent_trap_frame);

#endif
