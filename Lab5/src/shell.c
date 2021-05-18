#include "my_string.h"
#include "uart.h"
#include "utli.h"
#include "cpio.h"
#include "time.h"
#include "thread.h"
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
   
    else {
        uart_printf("shell: command not found: %s\n", cmd);
    }
}
