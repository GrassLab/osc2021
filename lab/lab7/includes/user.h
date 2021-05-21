
int atoi(char* str);
void test_arguments(int);
int getpid();
char* itoa(int num, char* str);
void user_delay(int);
char user_uart_getc();
int uart_read(char* buff, int size);
int uart_write(char* buff, int size);
extern void exec(char*, char**);
extern void exit();
extern void _uart_send(char);
extern void _uart_puts(char*);
extern char _uart_getc();
extern void print_hex(int);
extern int fork();
extern void test();

#define O_CREAT 1
#define DIR_MAX 10

extern int open(char* pathname, int flags);
extern int close(int fd);
extern int read(int fd, const void* buf, int count);
extern int write(int fd, void* buf, int count);
extern int readdir(char* dir, char* buff, int i);