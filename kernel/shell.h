#ifndef _LCD_SHELL_H_
#define _LCD_SHELL_H_

void shell_welcome_message();
void shell_start();

void command_controller(char *cmd);

void command_help();
void command_hello();
void command_reboot();

void command_cpio_info();
void command_ls();

void command_allocate();
void command_freez();
void command_freei();
void command_meminfo(int mode);
void command_kmalloc();
void command_kfree();

void command_load_user_program_to_addr();
void command_run_user_program();
void command_get_currentel();

void command_test();

void command_not_found();

#endif