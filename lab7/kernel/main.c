#include "uart.h"
#include "util.h"
#include "reboot.h"
#include "cpio.h"
#include "devicetree.h"
#include "buddy.h"
#include "allocator.h"
#include "exception.h"
#include "timer.h"
#include "thread.h"
#include "vfs.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
char last_cmd[CMDSIZE] = {0};
int cmdSize = 0;

int level = 0;
int isSync = 1;

int vfsIsInit = 0;

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
		uart_putstr("help     	    print all available commands \n");
		uart_putstr("hello    	    print Hello World! \n");
		uart_putstr("reboot   	    reboot raspi3 \n");
		uart_putstr("ls    	   	    list rootfs file \n");
		uart_putstr("cat      	    cat [filename] open and read file \n");
		uart_putstr("dtbls    	    list device tree \n");
		uart_putstr("dtbcat   	    cat [nodename] get property \n");
		uart_putstr("buddy    	    test buddy page frame allocator \n");
		uart_putstr("dynamic  	    test dynamic allocator\n");
		uart_putstr("curEL    	    get current expcetion level\n");
		uart_putstr("runUser 	    load and run a user program in the initramfs [svc.elf]\n");
		uart_putstr("userTimer	    load and run a user program in the initramfs [svc.elf] (coreTimer)\n");
		uart_putstr("asyncRead	    use uart interrupt and read \n");
		uart_putstr("asyncWrite	    use uart interrupt and write \n");
		uart_putstr("recover 	    disable interrupt \n");
		uart_putstr("setTimeout [MESSAGE] [SECONDS]   set timer timeout and print message \n");
		uart_putstr("threadreq1	    thread requirement 1\n");
		uart_putstr("threadreq2	    thread requirement 2\n");
		uart_putstr("vfsreq1 	    vfs requirement 1: Populate the root file system with initramfs\n");
		uart_putstr("vfsreq2 	    vfs requirement 2\n");
		uart_putstr("vfselec1 	    vfs elective 1\n");
		uart_putstr("vfselec2 	    vfs elective 2\n");
		uart_putstr("fatreq1 	    get FAT32 partition and mount the FAT32 File System\n");
		uart_putstr("fatreq2 	    add read and write in FAT32 \n");
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
			char *content = cpio_content(&cmd[4]);
			if(content)
				uart_putstr(content);
			else
				uart_putstr("file not found !");
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
			int ret = dtb_cat(&cmd[7], callback);
			if(ret == -1)
				uart_putstr("device not found !");
		}
		else
			uart_putstr("no input device name !");
		uart_putstr("\n");
	}	
	else if(strcmp(cmd, "buddy"))
	{
		buddy_test();
		uart_putstr("\n");
	}
	else if(strcmp(cmd, "dynamic"))
	{
		dynamic_test();
		uart_putstr("\n");
	}	
	else if(strcmp(cmd, "curEL"))
	{
		show_current_el();
		uart_putstr("\n");
	}
	else if(strcmpn(cmd, "runUser", 6))
	{
		// ex. runUser svc.elf		
		unsigned int length = strlen(cmd) - 8;
		if (length > 0)
			cpio_run_user_program(&cmd[8], 0);
		else
			uart_putstr("no input file name !");
		
		uart_putstr("\n");		
	}
	else if(strcmpn(cmd, "userTimer", 8))
	{
		uart_putstr("irq_handle\n");
		// ex. userTimer svc.elf		
		unsigned int length = strlen(cmd) - 10;
		if (length > 0)	
			cpio_run_user_program(&cmd[10], 1);
		else
			uart_putstr("no input file name !");
		
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "asyncRead"))
	{
		if(isSync == 1)
		{
			enable_interrupt();
			isSync = 0;
		}
		uart_putstr("start async read command...");
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "asyncWrite"))
	{
		if(isSync == 1)
		{
			enable_interrupt();
			isSync = 0;
		}
		uart_async_putstr("This is async write string...");
		uart_async_putstr("\n");		
	}
	else if(strcmp(cmd, "recover"))
	{
		if(isSync == 0)
		{
			disable_interrupt();
			isSync = 1;
			uart_putstr("disable_interrupt ...");
		}
		uart_putstr("\n");		
	}
	else if(strcmpn(cmd, "setTimeout", 9))
	{
		int second = 0;
		char msg[20] = {0};
		// get second and message
		// setTimeout [MESSAGE] [SECONDS]
		for(int i = 11; cmd[i] != '\0'; i++)
		{
			if(cmd[i] != ' ')
			{
				msg[i - 11] = cmd[i];
			}
			else
			{
				msg[i - 11] = '\0';
				
				for(int j = i + 1; cmd[j] != '\0'; j++)
				{
					if(cmd[j] >= '0' && cmd[j] <= '9') 
						second = second * 10 + cmd[j] - '0';
				}
			}
		}
		
		if(isSync == 1)
		{
			enable_interrupt();
			isSync = 0;
		}
		
		add_timer(print_timer_msg, msg, second);
	}
	else if(strcmp(cmd, "threadreq1"))
	{
		thread_test();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "threadreq2"))
	{
		thread_test2();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "vfsreq1"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		else
			uart_putstr("vfs inited ! \n");	
		
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "vfsreq2"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		thread_vfs_req2();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "vfselec1"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		thread_vfs_ele1();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "vfselec2"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		thread_vfs_ele2();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "fatreq1"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		thread_fat32_req1();
		uart_putstr("\n");		
	}
	else if(strcmp(cmd, "fatreq2"))
	{
		if(!vfsIsInit)
		{
			vfs_init();
			vfsIsInit = 1;
		}
		thread_fat32_req2();
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

int main()
{
	isSync = 1;
	
	cmd_init();
	uart_init();
	memory_init();
	init_timeout();
	init_thread();
	init_thread2();
	vfs_init();
	vfsIsInit = 1;
	
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
		if (isSync) // get char from user
			c = uart_getchar();
		else
			c = uart_async_getchar(); 
		
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
				if (isSync) // get char from user
					c2 = uart_getchar();
				else
					c2 = uart_async_getchar(); 
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
	
	return 0;
}
