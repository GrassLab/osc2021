# ifndef __USER_LIB__
# define __USER_LIB__

# define O_RD      4
# define O_WR      2
# define O_CREAT   1

# define F_RD      4
# define F_WR      2
# define F_EX      1

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

extern "C" unsigned long long svc_get_core_timer_value();
extern "C" unsigned long long svc_get_core_timer_ms();
extern "C" void set_timeout_svccall(struct one_shot_timer *n);
extern "C" int getpid();
extern "C" int uart_read(char *buf, int size);
extern "C" int uart_write(const char *buf, int size);
extern "C" int exec(const char *func, char **argv);
extern "C" int fork();
extern "C" void exit();
extern "C" int open(const char *pathname, int flags);
extern "C" int close(int fd);
extern "C" int write(int fd, const void *buf, int count);
extern "C" int read(int fd, void *buf, int count);
extern "C" int mkdir(const char *pathname);
extern "C" int chdir(const char *pathname);
extern "C" int mount(const char* device, const char* mountpoint, const char* filesystem);
extern "C" int umount(const char* mountpoint);

// my_math.h
void int_to_str(int n, char *s);
void int_to_hex(unsigned long long n, char *s);
void ptr_to_hex(void *p, char *s);
void int_to_hex_align(unsigned long long n, char *s, int len);
unsigned long hex_to_uint(char *s, int l);
int str_to_int(char *c);
int round_up(int n, int a);
int align_up(int n, int a);

// utli.h
void delay(int p);

// my_string.h
int str_cmp(const char *s1, const char *s2);
int str_len(const char *s);
void str_copy(const char *src, char *target);

void assert(bool t, const char *c);
# endif
