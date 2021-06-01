#ifndef _SYSCALL_H_
#define _SYSCALL_H_

void sys_exit(int status);
void do_exit(int status);
int sys_fork();
int do_fork();
int sys_exec(const char* name, char* const argv[]);
int do_exec(const char* name, char* const argv[]);
int sys_getpid();
int do_getpid();

void* load_program(const char* name);
void* fork_memcpy();
void* exec_set_argv(void* stack, int argc, char* const argv[]);

int sys_open(const char *pathname, int flags);
int do_open(const char *pathname, int flags);

int sys_close(int fd);
int do_close(int fd);

int sys_write(int fd, const void *buf, int count);
int do_write(int fd, const void *buf, int count);

int sys_read(int fd, void *buf, int count);
int do_read(int fd, void *buf, int count);


#endif