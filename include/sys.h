#ifndef _SYS_H_
#define  _SYS_H_

#include "type.h"

#define SYS_GETPID     1
#define SYS_EXEC       2
#define SYS_UART_READ  3
#define SYS_UART_WRITE 4
#define SYS_FORK       5
#define SYS_EXIT       6
#define SYS_SLEEP      7
#define SYS_OPEN       8
#define SYS_CLOSE      9
#define SYS_WRITE      10
#define SYS_READ       11
#define SYS_LS         12
#define SYS_MKDIR      13
#define SYS_CHDIR      14
#define SYS_MOUNT      15
#define SYS_UMOUNT     16

typedef struct {
    uint64_t x[31];
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
} tframe_t;

void el0_svc_handler(tframe_t*);
int do_getpid(void);
size_t do_uart_read(char*, size_t);
size_t do_uart_write(const char*, size_t);
int do_exec(const char *, char *const[], int);

#endif
