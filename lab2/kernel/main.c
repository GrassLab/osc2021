#include "uart.h"
#include "util.h"
#include "reboot.h"
#include "cpio.h"
#include "devicetree.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
char last_cmd[CMDSIZE] = {0};
int cmdSize = 0;

char file[20] = {0};
int level = 0;

void cmd_init()
{
	cmdSize = 0;
	for(int i = 0; i < CMDSIZE; i++)
		cmd[i] = 0;
}

// callback for print device 
void callback(unsigned long int ptr, unsigned long int strptr) 
{
	level++;
	print_indent(level);
	uart_putstr((char *)ptr);
	uart_putstr("\n");
	
	ptr += align4(strlen((char *)ptr) + 1);

	while (1) 
	{
		unsigned int tag = convert_bigendian((char *)ptr);
		ptr += sizeof(unsigned int);
		
		if (tag == FDT_BEGIN_NODE)
		{
			level++;
			print_indent(level);
			uart_putstr((char *)ptr);
			uart_putstr("\n");
	
			ptr += align4(strlen((char *)ptr) + 1);
		}
		else if (tag == FDT_END_NODE)
		{
			level--;
			if (level == 0)
				break;
		}
		else if (tag == FDT_NOP)
			continue;
		else if (tag == FDT_PROP)
		{
			unsigned int len = convert_bigendian((char *)ptr); // length
			ptr += sizeof(unsigned int); // ptr + 4
			unsigned int nameoff = convert_bigendian((char *)ptr); // name offset
			ptr += sizeof(unsigned int);
			
			print_indent(level);
			uart_putstr("Property Name:");
			uart_putstr((char *)(strptr + nameoff));
			uart_putstr("\t");
			print_indent(level);
			uart_putstr("Property Data:");
			uart_putstr((char *)ptr);
			uart_putstr("\n");
			
			ptr += align4(len);
		}
		else if (tag == FDT_END)
			break;
	}
}

void cmd_handle() // parse command
{
	if(strcmp(cmd, "hello"))
	{
		uart_putstr("Hello World! \n");
	}
	else if(strcmp(cmd, "help"))
	{
		uart_putstr("help     print all available commands \n");
		uart_putstr("hello    print Hello World! \n");
		uart_putstr("reboot   reboot raspi3 \n");
		uart_putstr("ls    	 list rootfs file \n");
		uart_putstr("cat      cat [filename] open and read file \n");
		uart_putstr("dtbls    list device tree \n");
		uart_putstr("dtbcat   cat [nodename] get property \n");
	}
	else if(strcmp(cmd, "reboot"))
	{
		uart_putstr("reboot .... \n");
		raspi3_reboot(100);
		while(1);  // wait for reboot
	}
	else if(strcmp(cmd, "ls"))
	{
		cpio_list();
		uart_putstr("\n");
	}
	else if(strcmpn(cmd, "cat", 2))
	{
		// ex. cat init.txt		
		unsigned int length = strlen(cmd) - 4;
		if (length > 0)
		{
			for(int i = 4, j = 0; j < length; i++, j++)
			{
				file[j] = cmd[i];
			}
			file[length] = '\0';
			
			char *content = cpio_content(file);
			if(content)
				uart_putstr(content);
			else
				uart_putstr("file not found !");
			
			for(int i = 0; i < 20; i++)
				file[i] = 0;
		}
		else
			uart_putstr("no input file name !");
		
		uart_putstr("\n");
		
	}
	else if(strcmp(cmd, "dtbls"))
	{
		dtb_ls();
		uart_putstr("\n");
	}
	else if(strcmpn(cmd, "dtbcat", 5))
	{
		level = 0;
		unsigned int length = strlen(cmd) - 7;
		if (length > 0)
		{
			for(int i = 7, j = 0; j < length; i++, j++)
			{
				file[j] = cmd[i];
			}
			file[length] = '\0';
			
			int ret = dtb_cat(file, callback);
			if(ret == -1)
				uart_putstr("device not found !");
			
			for(int i = 0; i < 20; i++)
				file[i] = 0;
		}
		else
			uart_putstr("no input device name !");
		uart_putstr("\n");
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
			case '\r':
			case '\n':	// 0X0A '\n' newline, parse command
				while(cmd[cmdSize] != 0)
					cmdSize++;
				cmd[cmdSize] = '\0';
				cmdSize++;
				uart_putstr("\n");				
				for(int i = 0; i < cmdSize; i++)  // store last command
					last_cmd[i] = cmd[i];	
				cmd_handle();
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
					for(int i = 0; i < cmdSize; i++)	// clear input
						uart_putstr("\b \b");
					cmd_init();
					for(int i = 0; i < CMDSIZE; i++)	// input last command
					{
						if(last_cmd[i] == 0)
							break;
						
						cmd[i] = last_cmd[i];
						uart_sendchar(last_cmd[i]);
						cmdSize++;
					}
				}
				else if (c2 == 'C' && cmdSize < strlen(cmd))	// cursor left
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
		
		if (cmdSize == CMDSIZE)
		{
			uart_putstr("\ncommand too long !\n# ");
			cmd_init();
		}
	}
}
