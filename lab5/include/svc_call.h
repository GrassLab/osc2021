# define PRINT_TIMEOUT_STR_BUFFER_LEN  20
# define PRINT_TIMEOUT_STR_LEN  50

void svc_init();
void svc_get_core_timer_value(unsigned long long *r);
void svc_get_core_timer_ms(unsigned long long *r);
void svc_set_timeout(char *c, unsigned int sec);
