#include "uart.h"
#include "utli.h"
#include "string.h"

void shell_init(){
   uart_init();
   uart_flush();
   uart_printf("\n\nRPI3 hello world :) \n");
}

void shell_input(char* cmd){
    uart_printf("\r #");
    int index = 0,end = 0;
    cmd[0] = '\0';
    char c,t;
    while((c = uart_read()) != '\n'){
        if(c == 27 ){
	    if((t = uart_read())=='['){
                char type = uart_read();
                if(type=='C'){
	    	    if(index < end) index++;
	        }else if(type == 'D'){
		    if(index > 0) index--;
	        }else if(type == '3'){
		    type = uart_read();
		    if(type == '~'){
	                for(int i = index; i < end; i++){
		            cmd[i] = cmd[i+1];
		        }
		        cmd[--end] = '\0';
	            }
	        }else{uart_flush();}
	    }else{uart_flush();}
        }else if(c == 3){
	    cmd[0] = '\0';
	    break;
	    // Control-C
	}else if(c == 8 || c == 127){
	    if(index >0){
	    	index--;
		for(int i = index; i < end; i++){
		    cmd[i] = cmd[i+1];
		}
		cmd[--end] = '\0';
	    }
	}else{
	    if(index < end){
	    	for(int i = end;i > index;i--){
		    cmd[i] = cmd[i-1];
		}
	    }
	    cmd[index++] = c;
	    cmd[++end] = '\0';
	}
	uart_printf("\r# %s \r\e[%dC", cmd, index+2);
    }
    uart_printf("\n");
}

void shell_controller(char* cmd){
    if(!strcmp(cmd,"")){return;}
    else if(!strcmp(cmd,"help")){
        uart_printf("help: print all available commands\n");
        uart_printf("hello: print Hello World!\n");
        uart_printf("reboot: reboot pi\n");
    }
    else if(!strcmp(cmd,"hello")){
        uart_printf("Hello World!\n");
    }
    else if(!strcmp(cmd,"reboot")){
        uart_printf("Rebooting...");
        reset();
        while (1); // hang cpu reboot
    }
    else{
        uart_printf("shell: command not found: %s\n", cmd);
    }
}



