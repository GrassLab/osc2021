#include "../include/uart.h"

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
    else if(!strcmp(buf, "loadimg")) command_loadimg();
    else if(!strcmp(buf, "jumpimg")) command_jumpimg();
    else command_notfound();

}

void command_help(){

    uart_puts("\n========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("help\t:\tprint all available commands.\n");
    uart_puts("loadimg\t:\tload img from uart.\n");
    uart_puts("jumpimg\t:\tjump to new kernel.\n");
    uart_puts("\n========================================\n");
}

void command_notfound(){

    uart_puts("Command not found.\t");
    uart_puts("Try <help>\n");

}

void command_loadimg(){

    int complete = 0;
    
    while(!complete){

        int size = 0;
        int n = 3000;

        uart_puts("Start Loading Kernel Image\n");
        uart_puts("Loading Kernel Image at address 0x10000\n");
        
        char *load_address = (char *)0x10000;
        
        uart_puts("Waiting img from uart...\n");

        // nop instructions for waiting uart
        while ( n-- ) {
            asm volatile("nop");
        }

        /* wating for uart input*/
        /* read kernel's size */
        // recv byte 2^8 = 256 
        size = uart_getc();
        size |= uart_getc() << 8;
        size |= uart_getc() << 16;
        size |= uart_getc() << 24;

        // over 2^20 bytes = 1 MB
        if (size < 64 || size > 1024*1024) {
            // size error
            uart_send('S');
            uart_send('E');            
            uart_send('\r');
            uart_send('\n');
            continue;
        }
        uart_send('O');
        uart_send('K');
        uart_send('\r');
        uart_send('\n');

        /* start receiving img */
        while (size--) {
            *load_address++ = uart_getc();
        }
        uart_puts("Load kernel at: 0x10000\n");
        complete = 1;

    }
}

void command_jumpimg(){

    uart_puts("Jump to new kernel\n");
    asm volatile (
		"mov x30, 0x10000;" // x30 register store return addr 
		"ret"
	);

}

void boot_shell(){

    uart_puts("$ ");

    char buf[32];
    int count = 0;

    while(1){

        /* get one-by-one input */
        /* until recv '\n' for finishing command */
        char c = uart_getc();  
        uart_send(c);
        if(c == '\n'){
            uart_send('\r');
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