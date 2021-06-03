#ifndef SHELL_H
#define SHELL_H






#define PM_RSTC         ((volatile unsigned int*)0x3F10001C)
#define PM_WDOG         ((volatile unsigned int*)0x3F100024)
#define PM_PASSWORD     (0x5a000000)


#define MAX_BUFFER_LEN 128
#define SYS_CMD_NUM 13

#define  USER_NAME     "andy"
#define  MACHINE_NAME  "pi"


void shell_welcome();
void shell();

#endif
