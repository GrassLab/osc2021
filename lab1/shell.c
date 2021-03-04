#include <stddef.h>
#include "miniuart.h"
#include "string.h"

#define BUFFER_MAX_SIZE 128
#define NEW_LINE "\r\n"

static int input(char *buffer){
    size_t size = 0;
    while(size < BUFFER_MAX_SIZE - 1){
        char c = miniuart_recv();
        switch(c){
        case 13:
            buffer[size] = '\0';
            miniuart_send_S(NEW_LINE);
            return 0;
        case 8:
        case 127:
            if(size>0){
                size--;
                miniuart_send_S("\b \b");
            }
            continue;
        default:
            buffer[size++] = c;
            break;
        }
        miniuart_send_C(c);
    }
    return 1;
}

static void help(){
    miniuart_send_S("Available commands:");
    miniuart_send_S(NEW_LINE);
    miniuart_send_S("\thelp");
    miniuart_send_S(NEW_LINE);
    miniuart_send_S("\thello");
    miniuart_send_S(NEW_LINE);
    miniuart_send_S("\treboot");
    miniuart_send_S(NEW_LINE);
}

static void hello(){
    miniuart_send_S("Hello World!");
    miniuart_send_S(NEW_LINE);
}

extern void reset();

static void exec(const char *command){
    if(strcmp("help", command) == 0){
        help();
    }else if(strcmp("hello", command) == 0){
        hello();
    }else if(strcmp("reboot", command) == 0){
        reset();
    }else{
        miniuart_send_S("unknown command: ");
        miniuart_send_S(command);
        miniuart_send_S(NEW_LINE);
    }
}

void shell(){
    char buffer[BUFFER_MAX_SIZE];
    while(1){
        miniuart_send_S("> ");
        input(buffer);
        exec(buffer);
    }
}
