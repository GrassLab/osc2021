#include "uart.h"

int strcmp ( char * s1, char * s2 ){
    int i;
    for (i = 0; i < strlen(s1); i++)
        if ( s1[i] != s2[i])
            return s1[i] - s2[i];

    return  s1[i] - s2[i];

}

int strlen ( char * s ){
    int i = 0;
    while (1){
        if ( *(s+i) == '\0' )
            break;
        i++;
    }

    return i;
}

void command(char buf[]){

    if(!strcmp(buf,"help")) command_help();
    else if(!strcmp(buf, "hello")) command_hello();
    else if(!strcmp(buf, "reboot")) command_reboot();
    else command_notfound();

}

void command_help(){

    uart_puts("\n========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("help\t:\tprint all available commands.\n");
    uart_puts("hello\t:\tprint Hello World!\n");
    uart_puts("reboot\t:\treboot raspi3.\n");
    uart_puts("\n========================================\n");
}

void command_hello(){

    uart_puts("Hello World!\n");

}

void command_reboot(){

    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;

}

void command_notfound(){

    uart_puts("Command not found.\t");
    uart_puts("Try <help>\n");

}

void my_shell(){

    uart_puts("$ ");

    char buf[32];
    int count = 0;

    while(1){

        char c = uart_getc();
        uart_send(c);
        if(c == '\n'){
            buf[count] = '\0';
            command(buf);
            for(int i=0;i<count;i++)
                buf[i] = "";
            count = 0;
            uart_puts("$ ");
        }
        else{
            buf[count] = c;
            count++;
        }    
    }

}