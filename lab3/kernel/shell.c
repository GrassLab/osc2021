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
static void testcode();
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
    {.name = "show_page_frame_bit", .func = &show_page_frame_bit, .usage = "show_page_frame_bit"},
    {.name = "show_free_page_list", .func = &show_free_page_list, .usage = "show_free_page_list"},
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

static void testcode(){
    char *str = malloc(10);
    if(str == NULL){
        return;
    }

    str[0] = 'v';
    str[1] = 's';
    str[2] = ' ';
    str[3] = 'c';
    str[4] = 'o';
    str[5] = 'd';
    str[6] = 'e';
    str[7] = ' ';
    str[8] = 's';
    str[9] = '\0';

    printf("sizeof=%ld" NEW_LINE, sizeof(((Buddy_System *)0)->malloc_page->state));

    printf("str = %p" NEW_LINE, str);
    miniuart_send_S(str);
    miniuart_send_S(NEW_LINE);
    mfree(str);
    return;

    char *first_page = palloc(1);
    char *end[2];
    for(int32_t i=0; i<2; i++){
        end[i] = palloc(5);
    }
    pfree(first_page);
    //pfree(end[0]);
    pfree(end[1]);
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
