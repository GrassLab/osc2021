int getpid();
int uart_read(char* buf,int size);
int uart_write(char* buf,int size);
unsigned int uart_printf(char* fmt,...);
int exec(char* name,char** argv);
void dumpState();
void cur_exit();
int sys_fork();
