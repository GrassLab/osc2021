#include "timer.h"
#include "printf.h"
#include "mm.h"
#include "uart.h"
#include "string.h"

void set_timeout()
{
    char second_string[10];
    printf("time: ");
    uart_getline(second_string);
    int second = atoi(second_string);

    char message[100];
    printf("message: ");
    uart_getline(message);

    struct user_timer *timer_object = km_allocation(sizeof(struct user_timer));

    timer_object->newest_system_time = 0;
    timer_object->trigger_time = second;
    strcpy(message, timer_object->message);
}

void print_time_stamp(unsigned long cntpct, unsigned long cntfrq)
{
    int timestamp = cntpct / cntfrq;
    printf("timestamp: %d\n", timestamp);
    return;
}