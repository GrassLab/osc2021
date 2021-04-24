extern int get_pid(void);
// extern void exec(unsigned long func, char **argv);
extern void exec(char *filename, char **argv);
extern int fork(void);
extern void exit(void);
extern int uart_read(char buf[], int size);
extern int uart_write(const char buf[], int size);