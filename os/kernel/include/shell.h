#ifndef __SHELL_H_
#define __SHELL_H_

int cmd_help();
int cmd_hello();
int cmd_reboot();
int cmd_ls();
int cmd_cat();
int load_kernel();
int cmd(const char *);
void shell();

#endif // __SHELL_H_
