#include "uart.h"
#include "util.h"
#include "sys.h"

int main(int argc, char **argv) 
{
    uart_init();
	
	// 1. print parent ID
	char buf[16] = {0};
	uart_putstr("Argv Test, pid ");
	unsignedlonglongToStr(call_sys_gitPID(), buf);
	uart_putstr(buf);
	uart_putstr("\n");

	// 2. prints the arguments
	uart_putstr("arguments: ");
    for (int i = 0; i < argc; ++i) 
	{
		uart_putstr(argv[i]);
		uart_putstr(", ");
    }
	uart_putstr("\n");
    
	// 3. exec fork_test.img
    char *fork_argv[] = {"fork_test", 0};	
    call_sys_exec("fork_test.img", fork_argv);

    return 0;
}
