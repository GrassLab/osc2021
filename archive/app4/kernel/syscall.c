#include "printf.h"
#include "syscall.h"

int getpid(){
	long ret;
	asm volatile("\
		mov w8, 1 \n\
		svc #0 \n");
}

int uart_read(char* buf,int size){
	long ret;
	asm volatile("\
		mov w8, 2 \n\
		svc #0 \n");
}

void uart_write(char* buf,int size){
	long ret;
	asm volatile("\
		mov w8, 3 \n\
		svc #0 \n");
}

void uart_printf(char* fmt,...){
	char dst[100];
    __builtin_va_list args;
    __builtin_va_start(args,fmt);
    unsigned int ret=vsprintf(dst,fmt,args);
    uart_write(dst,ret);
}

int exec(char* name,char** argv){
	long ret;
	asm volatile("\
		mov w8, 4\n\
		svc #0\n");
}

void exit(){
	asm volatile("\
		mov w8, 5\n\
		svc #0\n");
	while(1){}
}

int fork(){
	long ret;
	asm volatile("\
		mov w8, 6\n\
		svc #0\n");
}

int open(const char *pathname, int flags) {
	long ret;
	asm volatile("\
		mov w8, 7\n\
		svc #0\n");
}

int close(int fd) {
	long ret;
	asm volatile("\
		mov w8, 8\n\
		svc #0\n");
}

int write(int fd, const void *buf, int count) {
	long ret;
	asm volatile("\
		mov w8, 9\n\
		svc #0\n");
}

int read(int fd, void *buf, int count) {
	long ret;
	asm volatile("\
		mov w8, 10\n\
		svc #0\n");
}