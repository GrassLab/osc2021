#include "shell.h"

#include "mini_uart.h"
#include "string.h"
#include "vfs.h"
#include "printf.h"
#include "fat32.h"

struct cmd{
    char input[20];
    char description[30];
};

struct cmd cmd_list[SYS_CMD_NUM] = {
    {.input = "help", 		.description="List commands."},
    {.input = "hello", 		.description="Print hello world."},
    {.input = "reset", 		.description="Reset raspi3."},
    {.input = "ls", 		.description="List File in SD card."},
	{.input = "sd", 		.description="Test Fat read/write."}
};


void help(){
    uart_send_string("*********************************************\r\n");
    uart_send_string("[Command]\t\t[Description]\r\n");
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        uart_send_string(cmd_list[i].input);
        uart_send_string("\t\t");
        uart_send_string(cmd_list[i].description);
        uart_send_string("\r\n");
    }
	uart_send_string("*********************************************\r\n");
}

void hello(){
    uart_send_string("Hello World !\r\n");
}

void reset(int tick){ // reboot after watchdog timer expire
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | tick; // number of watchdog tick
  while(1);
}

void cancel_reset() {
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}


//Lab8 2-2
void sdcard(){

	struct file *a = vfs_open("TEST", REG_FILE);
	vfs_write(a, "OSC", 4);
	vfs_close(a);

	a = vfs_open("TEST", REG_FILE);

	int sz;
	char buf[100];
	sz  = vfs_read(a, buf, 100);
	printf("%s\r\n", buf);

}


void shell_command(char* buf){
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
	else if(strcmp(buf, "ls") == 0)				fat_listdirectory();
	else if(strcmp(buf, "sd") == 0)				sdcard();
	else if(strcmp(buf, "mbr") == 0)			fat_getpartition();
	else if(strcmp(buf, "dump") == 0)			memory_dump();
}


void shell_welcome(){
	uart_send_string(" _   _  ______   ___   _    ___  ____   ____ \r\n");
	uart_send_string("| \\ | |/ ___\\ \\ / / | | |  / _ \\/ ___| / ___|\r\n");
	uart_send_string("|  \\| | |    \\ V /| | | | | | | \\___ \\| |   \r\n");    
	uart_send_string("| |\\  | |___  | | | |_| | | |_| |___) | |___\r\n");
	uart_send_string("|_| \\_|\\____| |_|  \\___/   \\___/|____/ \\____|\r\n\r\n");
}


void shell(){
    shell_welcome();

    uart_send_string(USER_NAME);
    uart_send_string("@");
    uart_send_string(MACHINE_NAME);
	uart_send_string("$ ");

    char input_buffer[MAX_BUFFER_LEN];
    char *buf_ptr = input_buffer;
    memset (buf_ptr, '\0', MAX_BUFFER_LEN);   
    char c;
    while(1){
        c = uart_recv();
        if(c == '\r'){
            uart_send_string("\r\n");
            shell_command(input_buffer);
            
			uart_send_string(USER_NAME);
			uart_send_string("@");
			uart_send_string(MACHINE_NAME);
			uart_send_string("$ ");
            buf_ptr = input_buffer;
            memset(buf_ptr, '\0', MAX_BUFFER_LEN);
        }
        else if(c == '\b'){
            if(buf_ptr > input_buffer){
                uart_send_string("\b \b");
                *(--buf_ptr) = '\0';
            }
        }
        else{
            uart_send(c);
            *buf_ptr++ = c;
        }
    }


}

