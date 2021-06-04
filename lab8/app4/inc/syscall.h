#define O_CREAT 2
int getpid();
int uart_read(char* buf,int size);
int uart_write(char* buf,int size);
unsigned int uart_printf(char* fmt,...);
int exec(char* name,char** argv);
int open(char* pathname, int flags);
int write(int fd, char* input, int length);
int read(int fd, char* output, int length);
int close(int fd);
void dumpState();
void cur_exit();
int sys_fork();
