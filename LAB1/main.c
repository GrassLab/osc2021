#include "uart.h"

#define PM_PASSWORD     (0x5a000000)
#define PM_RSTC         ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
}

int strlen(char * s ) {
    int i = 0;
    while(1) {
        if ( *(s+i) == '\0' )
            break;
        i++;
    }
    return i;
}

int strcmp(char* s1, char* s2) {
    int i;

    for (i = 0; i < strlen(s1); i ++) {
        if ( s1[i] != s2[i]) {
            return s1[i] - s2[i];
        }
    }
    return  s1[i] - s2[i];
}

void command(char* str) {
    if(strcmp(str, "hello") == 0) {
        uart_puts("Hello World!\n");
    }
    else if(strcmp(str, "help") == 0) {
        uart_puts(
"+---------+------------------------------+\n\
| command |         Description          |\n\
+---------+------------------------------+\n\
| help    | print all available commands |\n\
| hello   | print Hello World!           |\n\
| reboot  | reboot machine               |\n\
+---------+------------------------------+\n"\
        );
    }
    else if(strcmp(str, "reboot") == 0) {
        reset(3);
    }
    else{
        uart_puts("Err: command ");
        uart_puts(str);
        uart_puts(" not found, try <help>\n");
    }
}

void strset (char * s1, int c, int size )
{
    int i;

    for ( i = 0; i < size; i ++)
        s1[i] = c;
}

void main() {
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");

    char buf[1000];
    strset(buf, 0, 1000);
    char input;
    int count = 0;

    uart_puts("# ");
    while (1) {
        input = uart_getc();
        uart_send(input);
        if(input == 10 || input == 13) {
            buf[count] = '\0';
            command(buf);
            strset(buf, 0, 1000);
            count = 0;
            uart_puts("# ");
        }
        else {
            buf[count] = input;
            count++;
        }
    }
}