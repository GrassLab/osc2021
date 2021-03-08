#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_RSTS ((volatile unsigned int *)(0x3F100020))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define SYS_CMD_NUM 3

void system_command(char*);
void sys_help();
void sys_hello();
void sys_reboot();
void reset(int );
void cancel_reset();
void *__memset(void*, int, int);
#endif
