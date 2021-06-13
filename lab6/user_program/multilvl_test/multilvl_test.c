#include "uart.h"
#include "util.h"
#include "sys.h"

int main(int argc, char **argv) 
{
    uart_init();
	
	uart_putstr("start multilevel test !\n");
	
	char buf[8];
	call_sys_mkdir("mnt");
	int fd = call_sys_open("/mnt/a.txt", O_CREAT);
	call_sys_write(fd, "Hi", 2);
	call_sys_close(fd);
	call_sys_chdir("mnt");
	fd = call_sys_open("./a.txt", 0);
	//assert(fd >= 0);
	if (!(fd >= 0))
	{
		uart_putstr("assert1, fd < 0 \n");
		return 0;
	}
	call_sys_read(fd, buf, 2);
	//assert(strncmp(buf, "Hi", 2) == 0);
	if (!(strncmp(buf, "Hi", 2) == 0)) 
	{
		uart_putstr("assert2, buf != Hi \n");
		return 0;
	}

	call_sys_chdir("..");
	call_sys_mount("tmpfs", "mnt", "tmpfs");
	fd = call_sys_open("mnt/a.txt", 0);
	//assert(fd < 0);
	if (!(fd < 0))
	{
		uart_putstr("assert3, fd >= 0 \n");
		return 0;
	}

	call_sys_umount("/mnt");
	fd = call_sys_open("/mnt/a.txt", 0);
	//assert(fd >= 0);
	if (!(fd >= 0))
	{
		uart_putstr("assert4, fd < 0 \n");
		return 0;
	}
	call_sys_read(fd, buf, 2);
	//assert(strncmp(buf, "Hi", 2) == 0);
	if (!(strncmp(buf, "Hi", 2) == 0)) 
	{
		uart_putstr("assert5, buf != Hi \n");
		return 0;
	}
	
	uart_putstr("end multilevel test !\n");
	
	call_sys_exit();
	
    return 0;
}
