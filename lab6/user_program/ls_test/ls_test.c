#include "uart.h"
#include "util.h"
#include "sys.h"

char buf[16] = {0};

int main(int argc, char **argv) 
{
    uart_init();
	
	uart_putstr("List file in directory: ");
	uart_putstr(argv[1]);
	uart_putstr("\n");
	
	int fd = call_sys_open(argv[1], 0);
	char name[100];
	int size;
	// Modify the for loop to iterate the directory entries of the opened directory.
	for(int i = 0;; ++i) 
	{
		size = call_sys_list(fd, name, i);
		
		if(size > 0)
		{
			//printf("Name: %s Size: %d\n", name, size);
			uart_putstr("Name: ");
			uart_putstr(name);
			uart_putstr(" Size: ");
			unsignedlonglongToStr(size, buf);
			uart_putstr(buf);
			uart_putstr("\n");
		}
		else if (size < 0)
			break;
	}
	
	call_sys_exit();
	
    return 0;
}
