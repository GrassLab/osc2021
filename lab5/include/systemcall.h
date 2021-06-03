#ifndef __SYSTEMCALL__
#define __SYSTEMCALL__

#define SYS_GETPID  1
#define SYS_READ  2
#define SYS_WRITE  3
#define SYS_EXEC  4
#define SYS_EXIT  5
#define SYS_TIME  6
#define SYS_FORK  7

int getpid();
int uart_read(char buf[], int size);
int uart_write(const char buf[], int size);
unsigned long svc_handler(unsigned long x0, unsigned long x1,
                 unsigned long x2, unsigned long x3,
                 unsigned long x4, unsigned long x5, unsigned long x6);

#endif