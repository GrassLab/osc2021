#ifndef SHELL_H
#define SHELL_H
#include <string.h>
#include "reset.h"
#include <uart.h>
#include <types.h>
#include <cpio.h>
#define BOOTLOADER_OFFSET 0x10000
void shell();
void do_command(char* command);
void loadimg();
__attribute__((__section__ (".bootloader"))) void readimg_jump(size_t load_address, size_t img_size);

#endif
