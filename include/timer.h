#ifndef	_TIMER_H
#define	_TIMER_H

#include "types.h"
#include "list.h"
/**
 * 
 */
typedef struct timer_event {
    struct list_head list;

    uint64_t registerTime, prevUpdatedTime;
    uint64_t duration, remainingTime;

    void (*callback)(struct timer_event *);
    
    char args[30]; // input message in requirement
} timer_event_t;

void timerEvents_irq_handler();

void timer_list_init();

void add_timer(void (*callback)(timer_event_t *), char *args, uint64_t duration);

void InitalizeNewTimerEvent(timer_event_t *timerEvent, void (*callback)(timer_event_t *), char *args, uint64_t duration, uint64_t registerTime);

/**
 * Update all registered timer events
 * It's used for timer interrupt and add_timer() function.
 * 
 */
void UpdateTimerEvents(uint64_t system_time);

void AddNewTimerEvents(timer_event_t *timerEvent);

void setNextTimerEventInterrupt();

void dumpTimerEvent(timer_event_t *timerEvent);

void dumpTimerEventList();

void print_timeoutEventInfo(timer_event_t *timeout_event);

void print_timestamp();

uint64_t get_system_time();

// Functions in timer.S
void core_timer_enable();
void core_timer_disable();
void core_timer_handler();



#endif