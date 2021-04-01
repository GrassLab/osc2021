#include "allocator.h"
#include "cpio.h"
#include "mystring.h"
#include "uart.h"
#include "utils.h"

void shell_cmd(char* cmd) {
    if(!strcmp(cmd, "help")) {
        uart_putstr("help: print all available commands\n");
        uart_putstr("hello: print Hello World!\n");
        uart_putstr("reboot: reboot rpi\n");
        uart_putstr("loadimg: start loadimg from uart\n");
        uart_putstr("cpio: read cpio data\n");
        uart_putstr("alloc: alloc mem\n");
        uart_putstr("free: free mem\n");
    }
    else if(!strcmp(cmd, "hello")) {
        uart_putstr("Hello World!\n");
    }
    else if(!strcmp(cmd, "reboot")) {
        reset(100);
    }
    else if(!strcmp(cmd, "loadimg")) {
        loadimg();
    }
    else if(!strcmp(cmd, "cpio")) {
        print_cpio();
    }
    else if(!strncmp(cmd, "alloc", 5)) {
        int id;
        int size;
        char *p=cmd;
        char buf[100];
        int idx;
        while(*p++!=' ');
        idx = 0;
        while(*p!=' ') {
            buf[idx++] = *p++;
        }
        buf[idx] = '\0';
        id = atoi(buf);

        p++;
        idx = 0;
        while(*p && *p!=' ') {
            buf[idx++] = *p++;
        }
        buf[idx] = '\0';
        size = atoi(buf);
        uart_printf("id: %d, size: %d\n", id, size);
        kmalloc(id, size);
    }
    else if(!strncmp(cmd, "free", 4)) {
        char *p = cmd;
        while(*p++ != ' ');
        char buf[100];
        int idx=0;
        while(*p && *p!=' ') {
            buf[idx++] = *p++;
        }
        buf[idx] = '\0';
        int id = atoi(buf);
        uart_printf("id: %d\n", id);
        free(id); 
    }
    else if(!strcmp(cmd, "")) {
    }
    else {
        uart_putstr("Unknwon command!\n");
    }
}
