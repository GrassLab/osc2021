#include "uart.h"
#include "scheduler.h"
#include "cpio.h"
#include "vfs.h"

void x0Set(unsigned long v){
	unsigned long* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
	task[16]=v;
}

void sys_getpid() {
    x0Set(current->id);
    return;
}

void sys_uart_read(char *buf, int size) {
    uart_read_buff(buf, size, 1);
}

void sys_uart_write(char *buf, int size) {
    uart_puts(buf);
}

void sys_exec(char *name, char **argv) {
    load_user_program_withArgv(name, argv);
}

void sys_exit() {
    _exit();
}

void sys_fork() {
    x0Set(create_thread(0, 0, 0, 0));
    return;
}

int sys_open(const char *pathname, int flags) {
    thread* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
    int ret = -1;
    for(int i = 0; i < FD_MAX_SIZE; i++) {
        if(task->fd_table[i] == 0) {
            ret = i;
            task->fd_table[i] = vfs_open(pathname, flags);
            break;
        }
    }
    x0Set(ret);
    return ret;
}

int sys_close(int fd) {
    thread* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
    if(task->fd_table[fd]) {
        vfs_close(task->fd_table[fd]);
        task->fd_table[fd] = 0;
    }
    return 0;
}

int sys_write(int fd, const void *buf, int count) {
    thread* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
    if(task->fd_table[fd]) {
        x0Set(vfs_write(task->fd_table[fd], buf, count));
    }
    return 0;
}

void sys_read(int fd, void *buf, int count) {
    thread* task;
	asm volatile("mrs %0, tpidr_el1	\n":"=r"(task):);
    if(task->fd_table[fd]) {
        x0Set(vfs_read(task->fd_table[fd], buf, count));
    }
}

void * const sys_call_table[] = {sys_getpid, sys_getpid, sys_uart_read, sys_uart_write, sys_exec, sys_exit, sys_fork, 
                                 sys_open, sys_close, sys_write, sys_read};