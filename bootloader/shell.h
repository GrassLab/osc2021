#ifndef _LCD_SHELL_H_
#define _LCD_SHELL_H_

void shell_welcome_message();
void shell_start();

void command_controller(char *cmd);

void command_help();
void command_hello();
void command_reboot();
void command_load_image();
void command_jump_to_kernel();
void command_not_found();

#endif