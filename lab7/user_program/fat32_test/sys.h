#ifndef	_SYS_H
#define	_SYS_H

#define O_CREAT 1

int call_sys_uart_write(char buf[], unsigned int size);
int call_sys_uart_read(char buf[], unsigned int size);
int call_sys_getPID();
int call_sys_fork();
int call_sys_exec(const char* name, char* const argv[]);
void call_sys_exit();
int call_sys_open(const char* name, int flags);
int call_sys_close(int fd);
int call_sys_write(int fd, const void *buf, unsigned int len);
int call_sys_read(int fd, void *buf, unsigned int len);
int call_sys_list(int fd, void *buf, int index);
int call_sys_mkdir(const char *pathname);
int call_sys_chdir(const char *pathname);
int call_sys_mount(const char* device, const char* mountpoint, const char* filesystem); 
int call_sys_umount(const char* mountpoint);

#endif