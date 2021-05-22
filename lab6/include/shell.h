# define SHELL_MAX_ARGC 20

//void invoke_cmd(char *cmd);
void shell();

extern "C" unsigned long long svc_get_core_timer_value();
extern "C" unsigned long long svc_get_core_timer_ms();
