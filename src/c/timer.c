#include "timer.h"
#include "printf.h"
#include "mm.h"
#include "uart.h"
#include "string.h"

struct list_head user_timer_list;

void init_user_timer()
{
    list_init_head(&user_timer_list);
}

void set_new_timeout()
{
    char second_string[10];
    printf("time: ");
    uart_getline(second_string);
    int second = atoi(second_string);

    char message[100];
    printf("message: ");
    uart_getline(message);

    struct user_timer *new_timer = km_allocation(sizeof(struct user_timer));

    new_timer->trigger_time = second;
    strcpy(message, new_timer->message);
    unsigned int frequency;
    unsigned int time_stamp;
    asm volatile(
        "mrs %0, cntpct_el0 \n\t"
        "mrs %1, cntfrq_el0 \n\t"
        : "=r"(time_stamp), "=r"(frequency)
        :);
    unsigned int system_time = time_stamp / frequency;
    new_timer->newest_system_time = system_time;

    if (list_empty(&user_timer_list))
    {
        /*
        TODO: Set the core timer directly if there is no previously set one.
        */
    }
    else
    {
        for (struct list_head *object = user_timer_list.next; object != NULL; object = object->next)
        {
            struct user_timer *temp = (struct user_timer *)object;
            temp->trigger_time -= system_time - temp->newest_system_time;
            temp->newest_system_time = system_time;
        }

        struct user_timer *front = (struct user_timer *)user_timer_list.next;
        if (new_timer->trigger_time < front->trigger_time)
        {

            list_crop(&front->list, &front->list);
            km_free(front);
            list_add_head(&new_timer->list, &user_timer_list);
            /*
            TODO: Re-set the core timer to the newly created one. 
            */
        }
        else
        {
            /*
            TODO:   Insert the newly created timer into the list,
                    then set the core timer to the first one in the list.
            */
        }
    }
}

void print_time_stamp(unsigned long cntpct, unsigned long cntfrq)
{
    int timestamp = cntpct / cntfrq;
    printf("timestamp: %d\n", timestamp);
    return;
}