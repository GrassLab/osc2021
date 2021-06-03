#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	 11

#ifndef __ASSEMBLER__

void sys_write(char * buf);
int sys_fork();

#endif

#endif  /*_SYS_H */
