#ifndef _EXCEPTION_H
#define _EXCEPTION_H

void exec_in_el0(void *start_addr, void *end_addr);
void get_arg_and_do_exec();

#endif