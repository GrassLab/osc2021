# include "my_math.h"
# include "my_string.h"
# include "utli.h"

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
