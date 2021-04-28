#include "uart.h"
#include "scheduler.h"

int sys_getpid() {
    return current->id;
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

int sys_fork() {
    unsigned long stack = (unsigned long)kmalloc(THREAD_SIZE);
    if(!stack) {
        while(1);
    }
    return create_thread(0, 0, 0, stack);
}

void * const sys_call_table[] = {sys_getpid, sys_getpid, sys_uart_read, sys_uart_write, sys_exec, sys_exit, sys_fork};