#include "miniuart.h"
#include "string.h"
#include "cpio.h"
#include "dtb.h"
#include "allocator.h"
#include "printf.h"

#define BUFFER_MAX_SIZE 128
#define COMMAND_LIST_SIZE (sizeof(COMMAND_LIST)/sizeof(struct Command))

static void help();
static void hello();
void testcode();
extern void reset();

struct Command {
    char *name;
    void (*func)();
    char *usage;
} static const COMMAND_LIST[] = {
    {.name = "help", .func = &help, .usage = "help"},
    {.name = "hello", .func = &hello, .usage = "hello"},
    {.name = "reboot", .func = &reset, .usage = "reboot"},
    {.name = "ls", .func = &ls, .usage = "ls"},
    {.name = "cat", .func = &cat, .usage = "cat filename"},
    {.name = "dtb_dump", .func = &dtb_dump, .usage = "dtb_dump [nodename[@address]]"},
    {.name = "show_free_page_list", .func = &show_free_page_list, .usage = "show_free_page_list"},
    {.name = "load", .func = &load, .usage = "load filename"},
    {.name = "run", .func = &testcode, .usage = "run"}
};

static uint32_t input(char *buffer){
    size_t len = 0;
    while(len < BUFFER_MAX_SIZE - 1){
        char c = miniuart_recv();
        switch(c){
        case 13:
            buffer[len] = '\0';
            miniuart_send_S(NEW_LINE);
            return 0;
        case 8:
        case 127:
            if(len > 0){
                len--;
                miniuart_send_S("\b \b");
            }
            continue;
        default:
            buffer[len++] = c;
            break;
        }
        miniuart_send_C(c);
    }
    return 1;
}

static void help(){
    miniuart_send_S("Available commands:" NEW_LINE);
    for(size_t i=0; i<COMMAND_LIST_SIZE; i++){
        miniuart_send_S(TAB);
        miniuart_send_S(COMMAND_LIST[i].usage);
        miniuart_send_S(NEW_LINE);
    }
}

static void hello(){
    miniuart_send_S("Hello World!" NEW_LINE);
}

void testcode(){
    char *p = palloc(1);
    char *m = malloc(10);
    printf("p = %p\n", p);
    printf("p = %p\n", m);
    free(p);
    free(m);
}

static void exec(char *command){
    char *cc = trim(command);
    char *arg = strtok(cc, ' ');
    for(size_t i=0; i<COMMAND_LIST_SIZE; i++){
        if(strcmp(cc, COMMAND_LIST[i].name) == 0){
            COMMAND_LIST[i].func(arg);
            return;
        }
    }
    miniuart_send_S("Error: Unknown command!!!" NEW_LINE);
}

void shell(){
    miniuart_send_S("This is shell" NEW_LINE);
    char buffer[BUFFER_MAX_SIZE];
    while(1){
        miniuart_send_S("> ");
        if(!input(buffer)){
            exec(buffer);
        }else{
            miniuart_send_S(NEW_LINE "Error: Input buffer overflow!!!" NEW_LINE);
        }
    }
}
