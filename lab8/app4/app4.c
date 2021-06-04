#include "inc/syscall.h"

int main(int argc,char** argv){
    char buffer[101];

    int fd=open(argv[1],0);
    uart_printf("\n");
    uart_printf("read somthing from %s(fd %d):\n",argv[1],fd);
    while(1){
        int ret=read(fd,buffer,100);
        if(ret==0)break;
        buffer[ret]=0;
        uart_printf("%s",buffer);
    }
    close(fd);

    fd=open(argv[1],0);
    uart_printf("\nwrite somthing to %s(fd %d):\n",argv[1],fd);
    int len=uart_read(buffer,100);
    write(fd,buffer,len);

    cur_exit();
}
