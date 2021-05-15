#pragma once

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)0x3F10001c)
#define PM_WDOG ((volatile unsigned int *)0x3F100024)

#define MAX_BUFFER_SIZE 64
char buffer[MAX_BUFFER_SIZE];
int buffer_pos;

void cmd_help();
void cmd_hello();
void cmd_reboot();
void cmd_ls();
void cmd_cat(char *pathname);
void cmd_dtb_print(int all);
void cmd_buddy_test();
void cmd_dma_test();
void cmd_load_user_program(const char *program_name);
void cmd_set_timeout(char *args);
void clear_buffer();
void receive_cmd();
void run_shell();
