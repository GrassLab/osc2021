#include "uart.h"
#include "util.h"
#include "sys.h"

char buff[16] = {0};

int main(int argc, char **argv) 
{
    uart_init();
	
	call_sys_mkdir("/fat");
	call_sys_mount("fatfs", "/fat", "fatfs");

	int fd = call_sys_open("/fat", 0);
	char name[100];
	int size;
	for (int i = 0;; ++i) 
	{
		size = call_sys_list(fd, name, i);
		
		if (size > 0)
		{
			//printf("Name: %s Size: %d\n", name, size);
			uart_putstr("Name: ");
			uart_putstr(name);
			uart_putstr(", Size: ");
			unsignedlonglongToStr(size, buff);
			uart_putstr(buff);
			uart_putstr("\n");
		}
		else if (size < 0)
			break;
	}
	
	call_sys_close(fd);

	call_sys_exit();
	
    return 0;
}
