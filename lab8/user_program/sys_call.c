#include "sys_call.h"

int getpid(){
    asm volatile("mov x22, #1");
    asm volatile("svc #0");
}

size_t uart_read(char buf[], size_t size){
    asm volatile("mov x22, #2");
    asm volatile("svc #0");
}

size_t uart_write(const char buf[], size_t size){
    asm volatile("mov x22, #3");
    asm volatile("svc #0");
}

int exec(const char* name, char *const argv[]){
    asm volatile("mov x22, #4");
    asm volatile("svc #0");
}

void exit(){
    asm volatile("mov x22, #5");
    asm volatile("svc #0");
}

int fork(){
    asm volatile("mov x22, #6");
    asm volatile("svc #0");
}

int open(const char *pathname, int flags){
    asm volatile("mov x22, #7");
    asm volatile("svc #0");
}

int close(int fd){
    asm volatile("mov x22, #8");
    asm volatile("svc #0");
}

int write(int fd, const void *buf, int count){
    asm volatile("mov x22, #9");
    asm volatile("svc #0");
}

int read(int fd, void *buf, int count){
    asm volatile("mov x22, #10");
    asm volatile("svc #0");
}
