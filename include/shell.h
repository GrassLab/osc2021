#ifndef _SHELL_H
#define _SHELL_H

void buffer_clear();
void execute_command(char *);
void init_shell();

void print_input_prompt();
void get_input();

void del_key();
void left_key();
void right_key();
void up_key();
void down_key();

void simple_shell();

void shell_hello();
void shell_help();
void shell_reboot();
void shell_boot_uart();

void shell_ls();

#endif