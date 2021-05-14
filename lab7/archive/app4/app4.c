#include "inc/syscall.h"

int main(int argc,char** argv){
    int fd=open(argv[1],0);
    char name[100];
    int size;
    while(1){
        size=read(fd,name,100);
        name[size]=0;
        if(size==0)break;
        uart_printf("Name: %s, Size: %d\n",name,size);
    }
    
	exit();
}