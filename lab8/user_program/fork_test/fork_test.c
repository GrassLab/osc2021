#include "util.h"
#include "sys.h"
#include "uart.h"

char buf[16] = {0};

void delay(int count)
{
	for(int i = 0; i < count; i++)
		asm volatile("nop");
}

void clear_buf()
{
	for(int i = 0; i < 16; i++)
		buf[i] = 0;
}

int main(void) 
{
    uart_init();
	
	// printf("Fork Test, pid %d\n", getpid());
	clear_buf();
	uart_putstr("Fork Test, pid ");
	unsignedlonglongToStr(call_sys_gitPID(), buf);
	uart_putstr(buf);
	uart_putstr("\n");
    
	
    int cnt = 1;
    int ret = 0;
    if ((ret = call_sys_fork()) == 0) // child
	{ 
		// printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
		clear_buf();
		uart_putstr("pid: ");
		unsignedlonglongToStr(call_sys_gitPID(), buf);
		uart_putstr(buf);
		uart_putstr(", cnt: ");
		unsignedlonglongToStr(cnt, buf);
		uart_putstr(buf);
		uart_putstr(", ptr: ");
		unsigned long long tmp = (unsigned long long)&cnt;
		unsignedlonglongToStrHex(tmp, buf);
		uart_putstr(buf);
		uart_putstr("\n");

        ++cnt;
        call_sys_fork();
        while (cnt < 5) 
		{
			// printf("pid: %d, cnt: %d, ptr: %p\n", getpid(), cnt, &cnt);
			clear_buf();
			uart_putstr("pid: ");
			unsignedlonglongToStr(call_sys_gitPID(), buf);
			uart_putstr(buf);
			uart_putstr(", cnt: ");
			unsignedlonglongToStr(cnt, buf);
			uart_putstr(buf);
			uart_putstr(", ptr: ");
			tmp = (unsigned long long)&cnt;
			unsignedlonglongToStrHex(tmp, buf);
			uart_putstr(buf);
			uart_putstr("\n");
			
            delay(1000000);
            ++cnt;
        }
    } 
	else 
	{
		// printf("parent here, pid %d, child %d\n", getpid(), ret);
		clear_buf();
		uart_putstr("parent here, pid ");
		unsignedlonglongToStr(call_sys_gitPID(), buf);
		uart_putstr(buf);
		uart_putstr(", child ");
		unsignedlonglongToStr(ret, buf);
		uart_putstr(buf);
		uart_putstr("\n");
    }
	
	clear_buf();
	uart_putstr("task ");
	unsignedlonglongToStr(call_sys_gitPID(), buf);
	uart_putstr(buf);
	uart_putstr(" exit! \n");
	
    call_sys_exit();
	
    return 0;
}