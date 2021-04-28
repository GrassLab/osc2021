#include "sys.h"

void sys_getpid(uint64_t *trap_frame){
    trap_frame[0] = current_thread()->pid;
}

void sys_uart_read(uint64_t *trap_frame){
    char *buffer = (char *)trap_frame[0];
    for(size_t i=0; i<trap_frame[1]; i++){
        buffer[i] = miniuart_recv();
    }
    trap_frame[0] = trap_frame[1];
}

void sys_uart_write(uint64_t *trap_frame){
    char *buffer = (char *)trap_frame[0];
    for(size_t i=0; i<trap_frame[1]; i++){
        miniuart_send_C(buffer[i]);
    }
    trap_frame[0] = trap_frame[1];
}

void sys_exec(uint64_t *trap_frame){
    trap_frame[0] = exec((char *)trap_frame[0], (char **)trap_frame[1]);
}

void sys_exit(uint64_t *trap_frame){
    exit();
}

void sys_fork(uint64_t *parent_trap_frame){
    uint64_t pid = fork();
    if(pid != 0){
        parent_trap_frame[0] = pid;

        Thread *parent = current_thread();
        Thread *child = get_thread(pid);

        uint64_t *child_trap_frame = (parent_trap_frame - (uint64_t *)parent->kernel_stack) + (uint64_t *)child->kernel_stack;
        child_trap_frame[0] = 0;
        child_trap_frame[29] = (parent_trap_frame[29] - (uint64_t)parent->user_stack) + (uint64_t)child->user_stack;
        if(child->user_stack){
            child_trap_frame[30] = (parent_trap_frame[30] - (uint64_t)parent->user_stack) + (uint64_t)child->user_stack;
        }
        child_trap_frame[33] = (parent_trap_frame[33] - (uint64_t)parent->user_stack) + (uint64_t)child->user_stack;
    }
}
