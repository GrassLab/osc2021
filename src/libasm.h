#ifndef LIBASM_H
#define LIBASM_H

void raise_int (int num);

int read (int fd, char *source, unsigned long size);
int write (int fd, char *source, unsigned long size);
void nanosleep (unsigned long time);
int get_pid ();
int fork ();
/* release cpu resource */
void yield ();
void api_test();
int thread (void *addr);
#endif
