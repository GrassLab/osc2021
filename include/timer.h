#include "list.h"

struct user_timer
{
    struct list_head list;

    unsigned long newest_system_time;
    unsigned int trigger_time;

    char message[50];
};

void set_timeout();
void print_time_stamp(unsigned long cntpct, unsigned long cntfrq);