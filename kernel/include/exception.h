#ifndef _EXCEPTION_H
#define _EXCEPTION_H

void exec_in_el0(void *start_addr, void *end_addr);
void get_arg_and_do_exec();
void get_arg_and_do_open();
void get_arg_and_do_read();
void get_arg_and_do_close();
void get_arg_and_do_write();


#endif