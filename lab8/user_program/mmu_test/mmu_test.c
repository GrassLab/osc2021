#include "util.h"
#include "sys.h"
#include "uart.h"

char buf[16] = {0};

void delay(int count)
{
	for(int i = 0; i < count; i++)
		asm volatile("nop");
}

int main(void) 
{
    uart_init();
	
	int cnt = 0;
    if(call_sys_fork() == 0) 
	{
		call_sys_fork();
		call_sys_fork();
		while(cnt < 10) 
		{
			uart_putstr("pid: ");
			unsignedlonglongToStr(call_sys_gitPID(), buf);
			uart_putstr(buf);
			uart_putstr(", sp: ");
			unsigned long long tmp = (unsigned long long)&cnt;
			unsignedlonglongToStrHex(tmp, buf);
			uart_putstr(buf);
			uart_putstr(" cnt: ");
			unsignedlonglongToStr(cnt++, buf);
			uart_putstr(buf);
			uart_putstr("\n");
			
			//printf("pid: %d, sp: 0x%llx cnt: %d\n", call_sys_getpid(), &cnt, cnt++); // address should be the same, but the cnt should be increased indepndently
			delay(1000000);
		}
	} 
	else 
	{
		int* a = 0x0; // a non-mapped address.
		//printf("%d\n", *a); // trigger simple page fault.
		//printf("Should not be printed\n");
		
		unsignedlonglongToStr(*a, buf);
		uart_putstr(buf);
		uart_putstr("\n");
		uart_putstr("Should not be printed\n");
	}
	
    call_sys_exit();
	
    return 0;
}