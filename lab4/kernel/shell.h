#ifndef SHELL_H
#define SHELL_H

#include <types.h>

#define BOOTLOADER_OFFSET 0x10000
#define SHELL_COMMNAD_SIZE 0x100

void shell();
void do_command(char* command);
void loadimg();
__attribute__((__section__ (".bootloader"))) void readimg_jump(void* load_address, size_t img_size, size_t dtb_address);

#endif
