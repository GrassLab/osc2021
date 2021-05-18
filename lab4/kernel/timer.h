#ifndef TIMER_H
#define TIMER_H

extern void core_timer_enable();
extern void core_timer_disable();

unsigned long long get_excute_time();
void set_next_timeout(unsigned int second);
void core_timer_handle();


#define max_queue_size		30
#define msg_size			20	

typedef struct timeout
{
    unsigned long long startTime;
	unsigned long long duration;
    char msg[msg_size];
    void (*callback)(char *);
    struct timeout *next;
} timeout;

void init_timeout();
void add_timer(void (*callback)(char *), char *msg, int duration);
void print_timer_msg(char *msg);
void timout_handle();

#endif