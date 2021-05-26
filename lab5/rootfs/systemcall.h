#ifndef __SYSTEMCALL__
#define __SYSTEMCALL__

int getpid();
int uart_read(char buf[], int size);
int uart_write(const char buf[], int size);
int exec(const char* name, char *const argv[]);
void exit();
unsigned long time();
int fork();

#endif