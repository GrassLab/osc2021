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
    unsigned long frequency;
    unsigned long time_stamp;
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
        list_add_head(&new_timer->list, &user_timer_list);
        asm volatile(
            "mov x0, 1              \n\t"
            "msr cntp_ctl_el0, x0   \n\t"
            "msr cntp_tval_el0, %0  \n\t"
            "mov x0, 2              \n\t"
            "ldr x1, =0x40000040    \n\t"
            "str w0, [x1]           \n\t"
            :
            : "r"(frequency * new_timer->trigger_time));
    }
    else
    {
        // update the system time for each timer in the list
        for (struct user_timer *temp = (struct user_timer *)user_timer_list.next; temp != NULL; temp = (struct user_timer *)temp->list.next)
        {
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
            asm volatile(
                "mov x0, 1              \n\t"
                "msr cntp_ctl_el0, x0   \n\t"
                "msr cntp_tval_el0, %0  \n\t"
                "mov x0, 2              \n\t"
                "ldr x1, =0x40000040    \n\t"
                "str w0, [x1]           \n\t"
                :
                : "r"(frequency * new_timer->trigger_time));
        }
        else
        {
            /*
            TODO:   Insert the newly created timer into the list,
                    then set the core timer to the first one in the list.
            */
            struct user_timer *current = front;
            while(1)
            {
                struct user_timer *next = (struct user_timer *)current->list.next;
                if (next->trigger_time < new_timer->trigger_time)
                {
                    current = next;
                    continue;
                }
                else
                {
                    __list_add(&new_timer->list, current->list.prev, current->list.next);
                    asm volatile(
                        "mov x0, 1              \n\t"
                        "msr cntp_ctl_el0, x0   \n\t"
                        "msr cntp_tval_el0, %0  \n\t"
                        "mov x0, 2              \n\t"
                        "ldr x1, =0x40000040    \n\t"
                        "str w0, [x1]           \n\t"
                        :
                        : "r"(frequency * front->trigger_time));
                    break;
                }
            }
        }
    }
}

void print_time_stamp(unsigned long cntpct, unsigned long cntfrq)
{
    int timestamp = cntpct / cntfrq;
    printf("timestamp: %d\n", timestamp);
    return;
}