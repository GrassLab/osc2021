#include "mm.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"
#include "string.h"
#include "exception.h"
/**
 * Implmentation of timer multiplexing using software mechanism and some common time functions like
 * get_system_time().
 * 
 * We implment timer multiplexing using software mechanism because the number of 
 * the hardware timer is limited
 *   
 * Baisc idea is that timers can be used to do periodic jobs such as scheduling and 
 * journaling and one-shot executing such as sleeping and timeout.
 * 
 * Note: 'cntp_tval_el0' [31:0] TimerValue is treated as a signed 32-bit integer,
 *       so setTimeout can't use a large timeout number (lower than 40 seconds is ok)
 */


struct list_head timer_list;

int isTimerMultiplexingEventIRQ = 0; // used for irq routing in exception.c

void timer_list_init()
{
    INIT_LIST_HEAD(&timer_list);
}

void timerEvents_irq_handler() {
    uint64_t system_time = get_system_time();
    // Update Timer events in list
    UpdateTimerEvents(system_time);

    timer_event_t *timeout_event = (timer_event_t *)timer_list.next;
    timeout_event->callback(timeout_event); // just print related info

    list_del(timer_list.next); // remove first timer event in timer list

    if (!list_empty(&timer_list))
        setNextTimerEventInterrupt(); // set new timer interrupt for next timer event if flag is 1
    else {
        core_timer_disable();
        isTimerMultiplexingEventIRQ = 0;
    }

    dumpyTimerEventList();
}

/**
 * @callback: Callback function execute when timeout
 * @args: Message
 * @duration: timeout seconds
 * 
 */

void add_timer(void (*callback)(timer_event_t *), char *args, uint64_t duration)
{
    timer_event_t *timerEvent = (timer_event_t *) kmalloc(sizeof(timer_event_t));
    
    uint64_t current_system_time = get_system_time();

    InitalizeNewTimerEvent(timerEvent, callback, args, duration, current_system_time);
    
    isTimerMultiplexingEventIRQ = 1;
    // We need to consider that when we add new timer event, if there are already other timer events
    // in timer_list. Then update all remaining time for each timer events and reset 'cntp_tval_el0' 
    // for next timer interrupt. 
    // 
    // First, update previously registered timer events in timer list. Second, add this new added
    // timer to timer list. It's worth noting that both of them should use same system time to 
    // avoid inconsistency problems.
    //
    // In following function, we will update shared variable access by interrupt hanlder, so
    // critical section is essential.
    //disable_irq(); 
    UpdateTimerEvents(current_system_time); 
    AddNewTimerEvents(timerEvent);
    setNextTimerEventInterrupt(); 
    //enable_irq();
    
}

void InitalizeNewTimerEvent(timer_event_t *timerEvent, void (*callback)(timer_event_t *), char *args, uint64_t duration, uint64_t registerTime)
{
    timerEvent->list.prev = NULL;
    timerEvent->list.next = NULL;
    timerEvent->callback = callback;
    timerEvent->registerTime = registerTime;
    timerEvent->prevUpdatedTime = registerTime;
    timerEvent->duration = duration;
    timerEvent->remainingTime = duration;

    // copy message
    int i;
    for (i = 0;i < strlen(args);i++) {
        timerEvent->args[i] = args[i]; 
    }
    timerEvent->args[i] = '\0';
    
}

/**
 * Update all registered timer events
 * It's used for timer interrupt and add_timer() function.
 */
void UpdateTimerEvents(uint64_t system_time)
{
    struct list_head *current;

    // Iterate over timer events list. Modify remaining time then update previous Updated Time.
    list_for_each(current, &timer_list) {
        timer_event_t *current_TE = (timer_event_t *) current;
        uint64_t temp = system_time - current_TE->prevUpdatedTime;
        current_TE->remainingTime = current_TE->remainingTime - temp;
        current_TE->prevUpdatedTime = system_time;
    }
}


/**
 * We insert the new timer event by ascending order according to remainingTime. So timer_list is always
 * sorted that make us easy to find the proper inserted position by linear search  
 */
void AddNewTimerEvents(timer_event_t *timerEvent)
{
    if (list_empty(&timer_list)) {
        // Timer list is empty, directly insert to a new one.
        list_add(&timerEvent->list, &timer_list);
    } else {
        struct list_head *current;
        
        // Because list is sorted, so just do linear search to find proper inserted position
        list_for_each(current, &timer_list) {
            // find the first greater one
            if (timerEvent->remainingTime < ((timer_event_t *) current)->remainingTime) {
                // Insert new timer event before the first greater one
                current = current->prev; 
                list_add(&timerEvent->list, current);
                break;
            }
            
        }
        
        // Edge case. If new timer event remainingTime is longest, add it to tail
        if (timerEvent->list.prev == NULL) {
            list_add_tail(&timerEvent->list, &timer_list);
        }
    }

    
    dumpyTimerEventList();
}

void setNextTimerEventInterrupt()
{
    timer_event_t *timeout_event = (timer_event_t *)timer_list.next;
    uint64_t next_timeout_duration = timeout_event->remainingTime;

    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq):);

    
    core_timer_enable();
    //printf("next_timeout_duration = %u\n", next_timeout_duration);
    //printf("freq * next_timeout_duration = %u\n", freq * next_timeout_duration);
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq * next_timeout_duration)); // set expired time
}

void dumpTimerEvent(timer_event_t *timerEvent)
{
    printf("TimerEvent info: \n");
    printf("*remaining time: %u\n", timerEvent->remainingTime);
    printf("registerTime: %u\n", timerEvent->registerTime);
    printf("prevUpdatedTime: %u\n", timerEvent->prevUpdatedTime);
    printf("duration: %u\n", timerEvent->duration);
    printf("callback(address): 0x{%x}\n", timerEvent->callback);
    printf("args(message): %s\n\n", timerEvent->args);
}

void dumpyTimerEventList()
{
    printf("--------Timer event list--------\n");
    if (!list_empty(&timer_list)) {
        struct list_head *pos;
        int i = 0;
        list_for_each(pos, &timer_list) {
            printf("Timer Event {Order:%d}\n", i++);
            dumpTimerEvent((timer_event_t *)pos);
        }
    } else {
        printf("timer event list is empty.\n");
    }
    printf("--------End Timer evnet list--------\n");
}

/* Callback function for timeout event */
void print_timeoutEventInfo(timer_event_t *timeout_event)
{
    printf("\n---Timeout event Info---\n");
    printf("Message: %s\n", timeout_event->args);
    printf("Duration Time: %u\n", timeout_event->duration);
    printf("Register Time: %u\n", timeout_event->registerTime);
    print_timestamp();
    printf("---End Info---\n");
}


uint64_t get_system_time()
{
    uint64_t cnt_freq, cnt_tpct;
    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r" (cnt_freq),  "=r" (cnt_tpct)
        :
    );
    uint64_t system_timestamp = cnt_tpct / cnt_freq;
    // printf("timestamp: %u\n", system_timestamp);
    return system_timestamp;
}

/**
 *  Write test cases
 */ 
void test() {

}