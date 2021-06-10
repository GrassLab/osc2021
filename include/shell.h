# define SHELL_MAX_ARGC 20

void shell();

extern "C" unsigned long long svc_get_core_timer_value();
extern "C" unsigned long long svc_get_core_timer_ms();
