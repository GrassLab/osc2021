#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_GETPID 39
#define SYS_FORK 57
#define SYS_EXECVE 59
#define SYS_EXIT 60
#define SVC_MASK 0xffff

#ifndef ASM
unsigned long sys_read(unsigned long fd, char *buf, unsigned long size);
unsigned long sys_write(unsigned long fd, const char *buf, unsigned long size);
#endif

#endif