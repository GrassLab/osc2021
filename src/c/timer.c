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
    unsigned long timestamp;
    asm volatile(
        "mrs %0, cntpct_el0 \n\t"
        "mrs %1, cntfrq_el0 \n\t"
        : "=r"(timestamp), "=r"(frequency)
        :);
    unsigned long system_time = timestamp / frequency;
    new_timer->newest_system_time = system_time;

    // if there is no previously set timer, then set it directly 
    if (list_empty(&user_timer_list))
    {
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
        for (struct user_timer *temp = (struct user_timer *)user_timer_list.next; temp != &user_timer_list; temp = (struct user_timer *)temp->list.next)
        {
            temp->trigger_time -= system_time - temp->newest_system_time;
            temp->newest_system_time = system_time;
        }

        struct user_timer *front = (struct user_timer *)user_timer_list.next;
        // overwrite cntp_tval_el0 if the trigger time of the new timer is less than that of the current one
        if (new_timer->trigger_time < front->trigger_time)
        {

            list_crop(&front->list, &front->list);
            km_free(front);
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
        // find the appropriate hole to insert the new timer
        else
        {
            struct user_timer *current = front;
            struct user_timer *next = (struct user_timer *)current->list.next;
            while ((next != &user_timer_list) && (next->trigger_time < new_timer->trigger_time))
            {
                next = (struct user_timer *)current->list.next;
                current = next;
            }
            __list_add(&new_timer->list, &current->list, current->list.next);
        }
    }

    return;
}

void handle_due_timeout()
{
    struct user_timer *front = (struct user_timer *)user_timer_list.next;
    printf("user timer due: %s\n", front->message);

    list_crop(&front->list, &front->list);
    km_free(front);

    if (!list_empty(&user_timer_list))
    {
        unsigned long frequency;
        unsigned long timestamp;
        asm volatile(
            "mrs %0, cntpct_el0 \n\t"
            "mrs %1, cntfrq_el0 \n\t"
            : "=r"(timestamp), "=r"(frequency)
            :);
        unsigned long system_time = timestamp / frequency;

        for (struct user_timer *temp = (struct user_timer *)user_timer_list.next; temp != &user_timer_list; temp = (struct user_timer *)temp->list.next)
        {
            temp->trigger_time -= system_time - temp->newest_system_time;
            temp->newest_system_time = system_time;
        }

        front = (struct user_timer *)user_timer_list.next;
        asm volatile(
            "msr cntp_tval_el0, %0  \n\t"
            :
            : "r"(frequency * front->trigger_time));
    }
    else
        core_timer_disable();

    return;
}

void print_timestamp(unsigned long cntpct, unsigned long cntfrq)
{
    int timestamp = cntpct / cntfrq;
    printf("timestamp: %d\n", timestamp);
    return;
}