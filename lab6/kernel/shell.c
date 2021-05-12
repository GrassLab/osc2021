#include "miniuart.h"
#include "string.h"
#include "cpio.h"
#include "dtb.h"
#include "allocator.h"
#include "printf.h"
#include "thread.h"
#include "elf.h"
#include "vfs.h"

#define BUFFER_MAX_SIZE 128
#define COMMAND_LIST_SIZE (sizeof(COMMAND_LIST)/sizeof(struct Command))

static void help();
static void hello();
static void pwd();
static void lab5();
static void lab6();
static void test_code();
extern void reset();
extern void delay(uint32_t cycle);

extern struct filesystem tmpfs;
extern struct filesystem cpiofs;

struct Command {
    char *name;
    void (*func)();
    char *usage;
} static const COMMAND_LIST[] = {
    {.name = "help", .func = help, .usage = "help"},
    {.name = "hello", .func = hello, .usage = "hello"},
    {.name = "reboot", .func = reset, .usage = "reboot"},
    {.name = "ls", .func = (void *)ls, .usage = "ls"},
    {.name = "cat", .func = (void *)cat, .usage = "cat filename"},
    {.name = "cd", .func = (void *)cd, .usage = "cd dirname"},
    {.name = "pwd", .func = pwd, .usage = "pwd"},
    {.name = "dtb_dump", .func = dtb_dump, .usage = "dtb_dump [nodename[@address]]"},
    {.name = "show_free_page_list", .func = show_free_page_list, .usage = "show_free_page_list"},
    {.name = "lab5", .func = lab5, .usage = "lab5"},
    {.name = "lab6", .func = lab6, .usage = "lab6"},
    {.name = "run", .func = test_code, .usage = "run"}
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

static void pwd(){
    printf("%s" NEW_LINE, get_pwd());
}

static void foo(){
    for(size_t i=0; i<10; i++) {
        printf("Thread id: %lu %lu" NEW_LINE, current_thread()->pid, i);
        delay(1000000);
        schedule();
    }
    //exit();
}

static void user_test(){
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    exec("argv_test", argv);
    //exec("preempt_test_delay.elf", NULL);
}

static void lab5(){
    // size_t N = 5;
    // for(size_t i=0; i<N; i++){
    //     thread_create(foo);
    // }
    thread_create(user_test);
    idle();
}

static void lab6_test(){
    exec("initramfs/vfs_test.elf", NULL);
}

static void lab6(char *path){
    thread_create(lab6_test);
    idle();
}

static void pwd_update(char *pathname){
    trim_multiple_of(pathname, '/');

    char *new = pathname;
    char *next = strtok(pathname + 1, '/');

    while(*new){
        new++;
    }

    char *component_name;
    while(next[0]){
        component_name = next;
        next = strtok(component_name, '/');

        printf("component_name = %s\n", component_name);
        printf("next = %s\n", next);

        if(!strcmp(component_name, ".")){
            continue;
        }

        *new++ = '/';
        while(*component_name){
            *new++ = *component_name++;
        }
    }


    printf("ans = %s\n", pathname);
}

static void test_code(){
    mkdir("/one");
    fopen("one/two", O_CREAT);
    fopen("one/two", O_CREAT);
    fopen("one/two/three", O_CREAT);
    mkdir("/one/two/three");
    mkdir("/one/three");
    ls("/one");
    ls("/");
    ls("/one/two");
    mkdir("/");
    fopen("/", 0);
}

static void run_command(char *command){
    char *cc = trim(command);
    char *arg = strtok(cc, ' ');
    for(size_t i=0; i<COMMAND_LIST_SIZE; i++){
        if(strcmp(cc, COMMAND_LIST[i].name) == 0){
            COMMAND_LIST[i].func(arg, NULL);
            return;
        }
    }
    miniuart_send_S("Error: Unknown command!!!" NEW_LINE);
}

void shell(){
    if(register_filesystem(&tmpfs) < 0){
        miniuart_send_S("Error: tmpfs register failed!!!" NEW_LINE);
    }else{
        vfs_mount(NULL, "/", tmpfs.name);
    }

    if(register_filesystem(&cpiofs) < 0){
        miniuart_send_S("Error: cpiofs register failed!!!" NEW_LINE);
    }else{
        cpiofs_populate(&cpiofs);
        mkdir("/initramfs");
        vfs_mount(NULL, "/initramfs", cpiofs.name);
    }

    miniuart_send_S("This is shell" NEW_LINE);
    char buffer[BUFFER_MAX_SIZE];
    while(1){
        miniuart_send_S("> ");
        if(!input(buffer)){
            run_command(buffer);
        }else{
            miniuart_send_S(NEW_LINE "Error: Input buffer overflow!!!" NEW_LINE);
        }
    }
}
