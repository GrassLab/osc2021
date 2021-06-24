#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_GETPID 39
#define SYS_FORK 57
#define SYS_EXECVE 59
#define SYS_EXIT 60
#define SYS_MKDIR 83
#define SYS_RMDIR 84
#define SYS_CREATE 85
#define SYS_REMOVE 86
#define SYS_CHDIR 87
#define SYS_MOUNT 88
#define SYS_UMOUNT 89
#define SYS_GETDENT 90
#define SVC_MASK 0xffff

#ifndef ASM
long sys_read(unsigned long fd, char *buf, unsigned long size);
long sys_write(unsigned long fd, const char *buf, unsigned long size);
#endif

#endif