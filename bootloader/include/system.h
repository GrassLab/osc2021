#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "string.h"
#include "uart.h"
#include "uart_boot.h"

#define SYS_CMD_NUM 4




void system_command(char*);

void help();
void hello();
void reset(int tick);
void cancel_reset();



#endif
