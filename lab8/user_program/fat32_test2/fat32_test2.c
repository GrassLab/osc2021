#include "uart.h"
#include "util.h"
#include "sys.h"

int main(int argc, char **argv) 
{
    uart_init();
	
	uart_putstr("start test fat32 read and write..");
	
	int size;
	char buf[200];
	
	// required 2-1 Look up and open a file in FAT32.
	uart_putstr("\nlook up and open FATTEST.TXT\n");
	int fd = call_sys_open("/fat/FATTEST.TXT", 0);
	
	// required 2-2 Read/Write a file in FAT32.
	uart_putstr("\nwrite FATTEST.TXT\n");
	call_sys_write(fd, "This is osc2021-lab7 test", 25);
	call_sys_close(fd);

	fd = call_sys_open("/fat/FATTEST.TXT", 0);
	size = call_sys_read(fd, buf, 200);
	buf[size] = '\0';
	uart_putstr(buf);
	uart_putstr("\n");
	
	call_sys_close(fd);

	call_sys_exit();
	
    return 0;
}
