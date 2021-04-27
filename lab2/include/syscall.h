#define SYS_GET_PID     1
#define SYS_EXEC        2
#define SYS_FORK        3
#define SYS_EXIT        4
#define SYS_UART_READ   5
#define SYS_UART_WRITE  6
#define SYS_SIGRETURN   7
#define SYS_KILL		8
#define SYS_SIGNAL 		9
#define SYS_USELESS		10

#define ESR_ISS_MASK ((1 << 25) - 1)

#define EAGAIN 0