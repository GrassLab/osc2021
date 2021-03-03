#include "uart.h"
#include "mystd.h"
#include "reboot.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
char last_cmd[CMDSIZE] = {0};
int cmdSize = 0;

void cmd_init()
{
	cmdSize = 0;
	for(int i = 0; i < CMDSIZE; i++)
		cmd[i] = 0;
}

void cmd_handle() // parse command
{
	uart_putstr("\r");

	if(strcmp(cmd, "hello"))
	{
		uart_putstr("Hello World! \n");
	}
	else if(strcmp(cmd, "help"))
	{
		uart_putstr("help     print all available commands \n");
		uart_putstr("hello    print Hello World! \n");
		uart_putstr("reboot   reboot raspi3 \n");
	}
	else if(strcmp(cmd, "reboot"))
	{
		uart_putstr("\rreboot .... \n");
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
	uart_putstr("\r");
	uart_putstr("      .~~.   .~~.       \n");
	uart_putstr("     '. \\ ' ' / .'     \n");
	uart_putstr("      .~ .~~~..~.       \n");
	uart_putstr("     : .~.'~'.~. :      \n");
	uart_putstr("    ~ (   ) (   ) ~     \n");
	uart_putstr("   ( : '~'.~.'~' : )    \n");
	uart_putstr("    ~ .~ (   ) ~. ~   NYCU OS 2021 \n");
	uart_putstr("     (  : '~' :  )    Welcome Raspberry Pi 3 !! \n");
	uart_putstr("      '~ .~~~. ~'       \n");
	uart_putstr("          '~'           \n");
	uart_putstr("# ");

	char c;
	char c2;
	while(1)
	{
		c = uart_getchar(); // get char from user
		
		// https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/45106/
		switch(c)
		{
			case '\n':	// 0X0A '\n' newline, parse command
				while(cmd[cmdSize] != 0)
					cmdSize++;
				cmd[cmdSize] = '\0';
				cmdSize++;
				uart_putstr("\r\n");
				cmd_handle();
				
				for(int i = 0; i < cmdSize; i++)  // store last command
					last_cmd[i] = cmd[i];
				
				cmd_init();
				break;
			case 127: // backspace
				if(cmdSize > 0)
				{
					cmdSize--;
					cmd[cmdSize] = 0;
					uart_putstr("\b \b");
				}
				break;
			case '[':
				c2 = uart_getchar();
				if (c2 == 'A')	// cursor up
				{
					for(int i = 0; i < cmdSize; i++) // clear input
						uart_putstr("\b \b");
					cmd_init();
					for(int i = 0; i < CMDSIZE; i++) // input last command
					{
						if(last_cmd[i] == 0)
							break;
						
						cmd[i] = last_cmd[i];
						uart_sendchar(last_cmd[i]);
						cmdSize++;
					}
				}
				else if (c2 == 'C' && cmdSize < strlen(cmd)) // cursor left
				{
					uart_putstr("\033[C");
					cmdSize++;
				}
				else if (c2 == 'D' && cmdSize > 0) // cursor right
				{
					uart_putstr("\033[D");
					cmdSize--;
				}
				break;
			default:
				if (c > 31 && c < 127)	// visible ascii
				{
					cmd[cmdSize] = c;
					cmdSize++;
					uart_sendchar(c);
				}
				break;
		}
		
		if (cmdSize >= CMDSIZE)	// cmd length > CMDSIZE
		{
			uart_putstr("\ncommand too long !\n# ");
			cmd_init();
		}
	}

}
