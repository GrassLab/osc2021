#define SYS_GET_PID     	1
#define SYS_EXEC        	2
#define SYS_FORK        	3
#define SYS_EXIT        	4
#define SYS_UART_READ   	5
#define SYS_UART_WRITE  	6
#define SYS_SIGRETURN   	7
#define SYS_KILL			8
#define SYS_SIGNAL 			9
#define SYS_USELESS			10
#define SYS_OPEN			11
#define SYS_READ   			12
#define SYS_WRITE 			13
#define SYS_CLOSE 			14
#define SYS_STAT_AND_NEXT   15
#define SYS_MKDIR			16
#define SYS_CHDIR			17
#define SYS_MOUNT			18
#define SYS_UMOUNT			19
#define SYS_SYNC			20
#define SYS_MKNOD			21

#define ESR_ISS_MASK ((1 << 25) - 1)

#define EAGAIN 0