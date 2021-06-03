#include "inc/syscall.h"

int main(int argc, char* argv[]){
    char buf[100];

    uart_printf("app3 test\n");
	int a=open("hello",O_CREAT);
    int b=open("world",O_CREAT);
    write(a,"Hello ",6);
    write(b,"World!",6);
    write(a,"Hello ",6);
    close(a);
    close(b);

    b=open("hello",0);
    a=open("world",0);
    int sz = 0;
    sz=read(b,buf,100);
    sz+=read(a,buf+sz,100);
    buf[sz]='\0';
    uart_printf("%s\n", buf); // should be Hello World!
    cur_exit();
    return 0;
}
