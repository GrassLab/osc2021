#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "utils.h"
#include "uart.h"
#include "uart_boot.h"
#include "cpio.h"
#include "dtb_parser.h"
#include "mm.h"

#define SYS_CMD_NUM 13

void system_command(char*);

void help();
void hello();
void reset(int tick);
void cancel_reset();



#endif
