# define PRINT_TIMEOUT_STR_BUFFER_LEN  20
# define PRINT_TIMEOUT_STR_LEN  50

void oneshot_timer_init();
extern "C" void set_timeout_svccall(struct one_shot_timer *n);
void svc_set_timeout(char *c, unsigned int sec);
