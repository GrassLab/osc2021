#include "my_string.h"
#include "uart.h"
#include "utli.h"
#include "cpio.h"
#include "time.h"
#include "thread.h"
#include "tmpfs.h"
extern char cpio_buf[];
enum ANSI_ESC {
    Unknown,
    CursorForward,
    CursorBackward,
    Delete
};
//ansi escape sequence
enum ANSI_ESC decode_csi_key() {
    char c = uart_read();
    if (c == 'C') {

        return CursorForward;
    }
    else if (c == 'D') {
        return CursorBackward;
    }
    else if (c == '3') {
        c = uart_read();
        if (c == '~') {
            return Delete;
        }
    }
    return Unknown;
}

enum ANSI_ESC decode_ansi_escape() {
    char c = uart_read();
    if (c == '[') {
        return decode_csi_key();
    }
    return Unknown;
}

void shell_init() {
    uart_init();
    uart_flush();
    uart_printf("\n\nHello From RPI3\n");
}

void shell_input(char* cmd) {
    uart_printf("\r# ");

    int idx = 0, end = 0, i;
    cmd[0] = '\0';
    char c;
    while ((c = uart_read()) != '\n') {
        // Decode CSI key sequences
        //uart_printf("%c", c);
        if (c == 27) {
            enum ANSI_ESC key = decode_ansi_escape();
            switch (key) {
                case CursorForward:
                    if (idx < end) idx++;
                    break;

                case CursorBackward:
                    if (idx > 0) idx--;
                    break;

                case Delete:
                    // left shift command
                    for (i = idx; i < end; i++) {
                        cmd[i] = cmd[i + 1];
                    }
                    cmd[--end] = '\0';
                    break;

                case Unknown:
                    uart_flush();
                    break;
            }
        }
        // CTRL-C
        else if (c == 3) {
            cmd[0] = '\0';
            break;
        }
        // Backspace
        else if (c == 8 || c == 127) {
            if (idx > 0) {
                idx--;
                // left shift command
                for (i = idx; i < end; i++) {
                    cmd[i] = cmd[i + 1];
                }
                cmd[--end] = '\0';
            }
        }
        else {
            // right shift command
            if (idx < end) {
                for (i = end; i > idx; i--) {
                    cmd[i] = cmd[i - 1];
                }
            }
            cmd[idx++] = c;
            cmd[++end] = '\0';
        }
        uart_printf("\r# %s \r\e[%dC", cmd, idx + 2);
    }

    uart_printf("\n");
}

void shell_controller(char* cmd) {
    if (!strcmp(cmd, "")) {
        return;
    }
    else if (!strcmp(cmd, "help")) {
        uart_printf("help: print all available commands\n");
        uart_printf("hello: print Hello World!\n");
        //uart_printf("timestamp: get current timestamp\n");
        uart_printf("reboot: reboot pi\n");
    }
    else if (!strcmp(cmd, "hello")) {
        uart_printf("Hello World!\n");
    }
    
    else if (!strncmp(cmd, "ls", 2)) {
        if(strlen(cmd) < 4){
            ls(cpio_buf);
        }
        else{
            const char *content = cpio_content(&cmd[3], cpio_buf);
            if(content){
                uart_printf("%s\n", content);
            }
            else{
                uart_printf("not found\n");
            }
        }
    }
    else if(!strcmp(cmd, "load")){
        load(cpio_buf);
    }
    else if(!strcmp(cmd, "lab5-1")){
        threadtest1();
        
    }
    else if(!strcmp(cmd, "lab5-2")){
        threadtest2();
    }
    else if (!strcmp(cmd, "reboot")) {
        uart_printf("Rebooting...");
        reset();
        while (1); // hang until reboot
    }
    else if(strcmp(cmd,"lab6-1")==0){
            char buf[100];
            //RW test
            file* f=vfs_open("dir/dirdir/f3",0);
            for(int i=0;i<10;++i)vfs_write(f,"12345",5);
            vfs_close(f);
            f=vfs_open("dir/dirdir/f3",0);
            int n=vfs_read(f,buf,100);
            buf[n]=0;
            uart_printf("%d: %s\n",n,buf);
            vfs_close(f);

            //create test
            file* a=vfs_open("dir/hello",O_CREAT);
            file* b=vfs_open("dir/world",O_CREAT);
            vfs_write(a,"Hello ",6);
            vfs_write(b,"World!",6);
            vfs_close(a);
            vfs_close(b);
            b=vfs_open("dir/hello",0);
            a=vfs_open("dir/world",0);
            int sz;
            sz=vfs_read(b,buf,100);
            sz+=vfs_read(a,buf+sz,100);
            buf[sz]=0;
            uart_printf("%s\n",buf);//should be Hello World!
            vfs_close(a);
            vfs_close(b);

            //ls test
            f=vfs_open("dir",0);
            while(1){
                n=vfs_read(f,buf,100);
                if(n==0)break;
                buf[n]=0;
                uart_printf("...%s\n",buf);
            }
            vfs_close(f);
    }
    else if(strcmp(cmd,"loadapp")==0){
        threadTest();
    }
    else if(strcmp(cmd,"lab7")==0){
            char buf[100];
            char buf2[100];
            //RW test

            
            
            file* f=vfs_open("TEST.TXT",0);
            int n=vfs_read(f,buf2,100);
            buf2[n]=0;
            uart_printf("%d: %s\n",n,buf2);
            for(int i=0;i<10;++i)vfs_write(f,"12345",5);
            vfs_close(f);
            f=vfs_open("TEST.TXT",0);
            n=vfs_read(f,buf,100);
            buf[n]=0;
            uart_printf("%d: %s\n",n,buf);
            vfs_sync(f);
            vfs_close(f);
        }
    else {
        uart_printf("shell: command not found: %s\n", cmd);
    }
}
