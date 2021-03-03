#include "../include/uart.h"
#include "../include/stringUtils.h"
#include "../include/shell.h"

#define MAX_BUF_SIZE 128
#define PM_PASSWORD 0x5a000000
#define PM_RSTC (volatile unsigned int*)0x3F10001c
#define PM_WDOG (volatile unsigned int*)0x3F100024



static char* commanlist[] = {"help" , "hello", "reboot"};

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
    for(int i =0 ; i < 2 ; ++i){
        uart_puts(commanlist[i]);
        uart_puts("\t");
    }
    uart_puts("\n");
}
static void read_input(char *buffer){
    int size = 0;
    while(size<MAX_BUF_SIZE){
        buffer[size] = uart_getc();
        uart_send(buffer[size]);
        if(buffer[size++] == '\n'){
            break;
        }
    }
    buffer[size] = '\0';
}


static void parse_input(char *buffer){
    if(*buffer == '\0'){
        return ;
    }else if(compString("help\n",buffer) == 0){
        help();
    }else if(compString("hello\n",buffer) == 0){
        hello();
    }else if(compString("reboot\n",buffer) == 0){
        reset();
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
