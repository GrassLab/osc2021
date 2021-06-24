#ifndef SYSCALL_H
#define SYSCALL_H

#define O_CREATE 1

int getpid();
int uart_read(char* buf,int size);
void uart_write(char* buf,int size);
void uart_printf(char* fmt,...);
int exec(char* name,char** argv);
void exit();
int fork();

int open(const char *pathname, int flags);
int close(int fd);
int write(int fd, const void *buf, int count);
int read(int fd, void *buf, int count);

#endif