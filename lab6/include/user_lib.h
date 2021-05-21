# ifndef __USER_LIB__
# define __USER_LIB__

# include "my_math.h"
# include "my_string.h"
# include "utli.h"
# include "flags.h"

extern "C" unsigned long long svc_get_core_timer_value();
extern "C" unsigned long long svc_get_core_timer_ms();
extern "C" void set_timeout_svccall(struct one_shot_timer *n);
extern "C" int getpid();
extern "C" int uart_read(char *buf, int size);
extern "C" int uart_write(char *buf, int size);
//extern "C" int exec(void (*func)(), char **argv);
extern "C" int exec(void (*func)(int, char**), char **argv);
extern "C" int fork();
extern "C" void exit();
extern "C" int open(const char *pathname, int flags);
extern "C" int close(int fd);
extern "C" int write(int fd, const void *buf, int count);
extern "C" int read(int fd, void *buf, int count);
extern "C" int mkdir(char *pathname);
extern "C" int chdir(char *pathname);
extern "C" int mount(const char* device, const char* mountpoint, const char* filesystem);
extern "C" int umount(const char* mountpoint);

static void assert(bool t, char *c){
  if (!t){
    uart_write("[ASSERT] ", 9);
    uart_write(c, str_len(c));
    uart_write("\n", 1);
    exit();
  }
}

# endif
