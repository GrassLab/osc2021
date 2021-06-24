#include "uart.h"
#include "util.h"
#include "reboot.h"
#include "kernel.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};

void cmd_init()
{
	for(int i = 0; i < CMDSIZE; i++)
		cmd[i] = 0;
}

void cmd_handle() // parse command
{
	if (strcmp(cmd, "help"))
	{
		uart_putstr("help     print all available commands \n");
		uart_putstr("loadimg  load new kernel from uart \n");
		uart_putstr("reboot   reboot raspi3 \n");
	}
	else if (strcmp(cmd, "loadimg"))
	{
		load_image();
	}
	else if(strcmp(cmd, "reboot"))
	{
		uart_putstr("reboot .... \n");
		raspi3_reboot(100);
		while(1);  // wait for reboot
	}
	else if(strlen(cmd) != 0)
	{
		uart_putstr("command \"");
		uart_putstr(cmd);
		uart_putstr("\" not found, try <help> \n");	
	}

	uart_putstr("# ");
}

void main()
{
	cmd_init();
	uart_init();
	
	// put welcome ascii art
	uart_putstr("\n");
	uart_putstr("      .~~.   .~~.       \n");
	uart_putstr("     '. \\ ' ' / .'     \n");
	uart_putstr("      .~ .~~~..~.       \n");
	uart_putstr("     : .~.'~'.~. :      \n");
	uart_putstr("    ~ (   ) (   ) ~     \n");
	uart_putstr("   ( : '~'.~.'~' : )    \n");
	uart_putstr("    ~ .~ (   ) ~. ~     \n");
	uart_putstr("     (  : '~' :  )    This is bootloader !! \n");
	uart_putstr("      '~ .~~~. ~'       \n");
	uart_putstr("          '~'           \n");
	uart_putstr("# ");

	while(1)
	{
        uart_read_cmd(cmd);
		cmd_handle();			
		cmd_init();
	}

}
