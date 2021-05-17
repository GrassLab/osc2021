#ifndef LIBASM_H
#define LIBASM_H

void raise_int (int num);

int read (int fd, char *source, unsigned long size);
int write (int fd, char *source, unsigned long size);
void nanosleep (unsigned long time);
int get_pid ();
int fork ();
int exec(char *f, char *args[]);
void exit (int flag);
/* release cpu resource */
void yield ();
void api_test();
int thread (void *addr);
void stack_info (unsigned long *stack, unsigned long *kstack, unsigned long *sp);
#endif
