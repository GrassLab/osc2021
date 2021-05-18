#ifndef _PROCESS_H
#define _PROCESS_H

struct Thread * create_process(void *source_addr, int size, int argc, char *argv[]);
void do_getpid();
void do_fork();
void do_exit();
#endif