#include "uart.h"
#include "util.h"
#include "sys.h"

char buf[100] = {0};

int main(int argc, char **argv) 
{
    uart_init();
	
	int a = call_sys_open("hello", O_CREAT);
	int b = call_sys_open("world", O_CREAT);
	call_sys_write(a, "Hello ", 6);
	call_sys_write(b, "World!", 6);
	call_sys_close(a);
	call_sys_close(b);
	b = call_sys_open("hello", 0);
	a = call_sys_open("world", 0);
	int sz;
	sz = call_sys_read(b, buf, 100);
	sz += call_sys_read(a, buf + sz, 100);
	buf[sz] = '\0';
	
	//printf("%s\n", buf); // should be Hello World!
	uart_putstr(buf);
	uart_putstr("\n");
	
	call_sys_exit();

    return 0;
}
