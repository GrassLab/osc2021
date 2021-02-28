#include "../include/uart.h"
#include "stringUtils.c"
#define MAX_BUF_SIZE 128
static char* commanlist[] = {"help","hello"};

static void printprompt(){
    uart_puts("~");
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
    }else{
        uart_puts("No Such Command\n");
        return ;
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
