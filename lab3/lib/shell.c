#include "../include/uart.h"
#include "../include/stringUtils.h"
#include "../include/shell.h"
#include "../include/utils.h"
#include "../include/initrd.h"
#include "../include/memAlloc.h"
#define MAX_BUF_SIZE 128
#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024



static char* commanlist[] = {"help" , "hello", "reboot", "loadimg", "find","my_alloc","my_free", "mem_status_dump","dy_mem_status_dump" };

void read_input(char *buffer){
    int size = 0;
    while(size<MAX_BUF_SIZE){
        buffer[size] = uart_getc();
        uart_send(buffer[size]);
        if(buffer[size++] == '\n'){
            break;
        }
    }
    buffer[--size] = '\0';
}

void reset(){ // reboot after watchdog timer expire
    int tick = 100;
    *(PM_RSTC) = PM_PASSWORD | 0x20; // full reset
    *(PM_WDOG) = PM_PASSWORD | tick; // number of watchdog tick

    while(1){};
}

static void printprompt(){
    uart_puts("~ ");
}

static void hello(){
    uart_puts("hello world!\n");
}

static void help(){
    uart_puts("Available Commands:\n");
    for(int i =0 ; i < 9 ; ++i){
        uart_puts(commanlist[i]);
        uart_puts("\t");
    }
    uart_puts("\n");
}


static void loadimg(){

	unsigned long k_addr=0,k_size=0;
	char c;
	uart_puts("Please enter kernel load address (Hex): ");

    char buffer[MAX_BUF_SIZE];

    read_input(buffer);
    k_addr = getHexFromString(buffer);
    uart_puts("\n");
	uart_puts("Please enter kernel size (Dec): ");
    read_input(buffer);
    k_size = getIntegerFromString(buffer);
    uart_puts("\n");

	uart_puts("Please send kernel image now...\n");
		unsigned char* target=(unsigned char*)k_addr;
		while(k_size--){
			*target=uart_getb();
			target++;
			uart_send('.');
		}

		uart_puts("loading...\n");
		asm volatile("br %0\n"::"r"(k_addr)); // (assembler template:output operand:input operand: clobber??) %0 is number of operand "r" means register
}


static void parse_input(char *buffer){
    if(*buffer == '\0'){
        return ;
    }else if(compString("help",buffer) == 0){
        help();
    }else if(compString("hello",buffer) == 0){
        hello();
    }else if(compString("reboot",buffer) == 0){
        reset();
    }else if(compString("loadimg",buffer) == 0){
        loadimg();
    }else if(compString("find",buffer) == 0){
        cpio();
    }else if(compString("my_alloc",buffer) == 0){
        uart_puts("Please enter size (Hex):");
        char size[MAX_BUF_SIZE];
        read_input(size);
        int int_size = getHexFromString(size);
        uart_puts("\n");
        my_alloc(int_size);
    }else if(compString("my_free",buffer) == 0){
        uart_puts("Please enter address:");
        char addr[MAX_BUF_SIZE];
        read_input(addr);
        int int_addr = getHexFromString(addr);
        uart_puts("\n");
        my_free(int_addr);
    }else if(compString("mem_status_dump",buffer) == 0){
        mem_status_dump();
    }else if(compString("dy_mem_status_dump",buffer) == 0){
        dy_mem_status_dump();
    }else{
        uart_puts("No Such Command\n");
    }

}
void shell(){
    char buffer[MAX_BUF_SIZE];
    while(1){
        printprompt();
        read_input(buffer);
        parse_input(buffer);
    }

}
