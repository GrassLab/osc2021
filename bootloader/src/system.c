#include "system.h"

struct cmd{
    char input[20];
    char description[30];
};


struct cmd cmd_list[SYS_CMD_NUM] = {
    {.input = "help", .description="List commands."},
    {.input = "hello", .description="Print hello world."},
    {.input = "reset", .description="Reset raspi3."},
    {.input = "loadimg", .description="Load new kernel."},
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
	
	
	if     (strcmp(buf, "help" ) == 0) 		help();
	else if(strcmp(buf, "hello") == 0)		hello();
	else if(strcmp(buf, "reset") == 0)		reset(100);
    else if(strcmp(buf, "loadimg") == 0)	loadimg();
}

void help(){
    uart_puts("\n\r###########################################\r\n");
    uart_puts("[Command]\t\t[Description]\r\n");
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        uart_puts(cmd_list[i].input);
        uart_puts("\t\t\t");
        uart_puts(cmd_list[i].description);
        uart_puts("\r\n");
    }
	uart_puts("###########################################\r\n\n\r");
}

void hello(){
    uart_puts("Hello World !\r\n");
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


