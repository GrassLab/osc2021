#include "system.h"




struct cmd{
    char input[20];
    char description[30];
};


struct cmd cmd_list[SYS_CMD_NUM] = {
    {.input = "help\t\t", 		.description="List commands."},
    {.input = "hello\t\t", 		.description="Print hello world."},
    {.input = "reset\t\t", 		.description="Reset raspi3."},
    {.input = "loadimg\t\t", 	.description="Load new kernel."},
    {.input = "ls\t\t", 		.description="List rootfs."},
    {.input = "cat [file]\t",	.description="Show content of file."},
    {.input = "dtb_ls\t\t",		.description="List device tree."},
    {.input = "dtb_cat\t\t",	.description="Parse device tree."},
    {.input = "info\t\t",		.description="Print buddy info."},
    {.input = "stat\t\t",		.description="Print buddy stat."},
    {.input = "slab\t\t",		.description="Print slab."},
	{.input = "load [app]\t",	.description="Load app."},
    {.input = "el\t\t",			.description="Print current el."},
    {.input = "async\t\t",		.description="Demo async."},
    {.input = "timeout [time]\t",	.description="Set timeout."},
};

void system_command(char* buf){
    char *args = buf;
    int arg_idx = 0;
    for(; buf[arg_idx] != '\0'; ++arg_idx, ++args){
        if(buf[arg_idx] == ' '){
            buf[arg_idx] = '\0';
            args = buf + arg_idx + 1;
            break;
        }
    }
	
	
	if     (strcmp(buf, "help" ) == 0) 			help();
	else if(strcmp(buf, "hello") == 0)			hello();
	else if(strcmp(buf, "reset") == 0)			reset(100);
    else if(strcmp(buf, "loadimg") == 0)		loadimg();
    else if(strcmp(buf, "ls") == 0)   			cpio_list();
	else if(strcmp(buf, "cat") == 0)   			cpio_cat(args);
	else if(strcmp(buf, "dtb_ls") == 0)   		print_dt_info();
	else if(strcmp(buf, "dtb_cat") == 0)   		parse_dt();
	else if(strcmp(buf, "info") == 0)   		print_buddy_info();
	else if(strcmp(buf, "stat") == 0)   		print_buddy_stat();
	else if(strcmp(buf, "slab") == 0)   		print_slab();
	else if(strcmp(buf, "load") == 0)   		load_app(args);
	else if(strcmp(buf, "el") == 0)   			print_el();
	else if(strcmp(buf, "async") == 0)   		test_uart_async();
	else if(strcmp(buf, "timeout") == 0)   		set_timeout(args);

}


void help(){
	           
    uart_puts("---------------------------------------------\n");
    uart_puts("[Command]\t[Description]\r\n");
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        uart_puts(cmd_list[i].input);
        uart_puts(cmd_list[i].description);
        uart_puts("\r\n");
    }
	uart_puts("---------------------------------------------\n");
}

void hello(){
    uart_puts("Hello World !\n");
}



void reset(int tick){ // reboot after watchdog timer expire
  uart_puts("Start Reset...\n");
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
  while(1);
}

void cancel_reset() {
  uart_puts("Cancel Rebooting...\n");
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}


