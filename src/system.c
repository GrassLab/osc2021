#include "system.h"
#include "string.h"
#include "uart.h"

struct cmd{
    char input[20];
    void(*callback)(void);
};
struct cmd cmd_list[3] = {
    {.input = "help", .callback = sys_help},
    {.input = "hello", .callback = sys_hello},
    {.input = "reboot", .callback = sys_reboot}
};
void system_command(char* buf){
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        if(strcmp(buf, cmd_list[i].input) == 0){
            cmd_list[i].callback();
            break;
        }
    }
    return;
}
void sys_help(){
    uart_puts("[Command] | [Description]\r\n");
    uart_puts(" help     |  print all available commands\r\n");
    uart_puts(" hello    |  print Hello World\r\n");
    uart_puts(" reboot   |  reboot raspi3\r\n");
}
void sys_hello(){
    uart_puts("Hello World !\r\n");
}
void sys_reboot(){
    reset(100);
    while(1);
}
void *__memset(void* buf, int c, int size){
    char *ptr = buf;
    while(size--)
        *ptr++ = c;
    return buf;
}
void reset(int tick){
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}
void cancel_reset(){
    *PM_RSTC = PM_PASSWORD | 0x00;
    *PM_WDOG = PM_PASSWORD | 0x00;

}
