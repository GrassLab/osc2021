#include "uart.h"
#include "util.h"
#include "timer.h"

/*
*	get_excute_time
*   
*   cntpct_el0: The timer’s current count.
*   cntfrq_el0: the frequency of the timer
*
*   excute time = cntpct_el0 / cntfrq_el0
*/
unsigned long long get_excute_time() 
{
	unsigned long long cntpct_el0, cntfrq_el0;
	asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct_el0));
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
	
	return cntpct_el0 / cntfrq_el0;
}

/*
*	set_next_timeout
*   
*   cntp_tval_el0: (cntp_cval_el0 - cntpct_el0). You can use it to set an expired timer after the current timer count.
*   cntfrq_el0: the frequency of the timer
*
*   we set cntp_cval_el0 = timer frequency * second = next interrupt timeout
*/
void set_next_timeout(unsigned int second) 
{
	unsigned long cntfrq_el0;
	asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq_el0));
	asm volatile("msr cntp_tval_el0, %0" : : "r"(cntfrq_el0 * second));
}

/*
*	core time interrupt handle   
*   
*   1. set next timeout = 2s later
*   2. print excute time
*/
void core_timer_handle()
{
	set_next_timeout(2);
	
	uart_putstr("seconds after booting: ");
	
	char buf[16] = {0};
	unsignedlonglongToStr(get_excute_time(), buf);
	uart_putstr(buf);	
	uart_putstr("seconds\n");
}

//////////////////////////////  timeout  ///////////////////////////////

timeout timeout_buffer[max_queue_size];
timeout *timeout_queue;
int buffer_index;

void init_timeout()
{
    for(int i = 0; i < max_queue_size; i++)
	{
        timeout_buffer[i].startTime = 0;
        timeout_buffer[i].duration = 0;
        timeout_buffer[i].next = 0;
    }
	
    timeout_queue = 0;   
    buffer_index = 0;
}
void add_timer(void (*callback)(char *), char* msg, int duration)
{	
	//
	// set new timeout (in last)
	//
	
	unsigned long long currentTime = get_excute_time();
    
	timeout_buffer[buffer_index].startTime = currentTime;
    timeout_buffer[buffer_index].duration = duration;
    timeout_buffer[buffer_index].callback = callback;	
    for(int i = 0; i < msg_size; i++)
	{
        timeout_buffer[buffer_index].msg[i] = msg[i];
        if(msg[i] == '\0') 
			break;
    }
	
	//
	// find proper location insert (by end time)
	//
	
	// 1. find position
    timeout *prev = 0, *cur = 0;
    for(cur = timeout_queue; cur != 0; prev = cur, cur = cur->next)
	{
        if(cur->startTime + cur->duration > currentTime + duration) 
			break;
    }
   
    // empty
    if(prev == 0 && cur == 0) 
	{
		core_timer_enable();
        timeout_queue = &timeout_buffer[buffer_index];
		set_next_timeout(timeout_queue->duration);
    }
	// new timeout end time > timeout_queue end time
    else if(prev != 0 && cur == 0) 
	{
		prev->next = &timeout_buffer[buffer_index];
	}
	// new timeout end time < timeout_queue end time
    else if (prev == 0 && cur != 0)
	{
        timeout_buffer[buffer_index].next = cur;
		timeout_queue = &timeout_buffer[buffer_index];
		set_next_timeout(timeout_queue->duration);
	}
	else
	{
		timeout_buffer[buffer_index].next = cur;
		prev->next = &timeout_buffer[buffer_index];         
    }
    
	// if large max size, index = 0
    for(int i = 0; i < max_queue_size && timeout_buffer[buffer_index].startTime > 0; i++)
	{
        if(++buffer_index == max_queue_size) 
			buffer_index = 0;
    }

}

/*
*	print_timer_msg   
*   
*   start excute time: 5s,  end excute time: 7s, duration time: 2s, message: text
*   
*/
void print_timer_msg(char* msg)
{
	char buf[16] = {0};
	
    uart_putstr("start excute time: ");
	unsignedlonglongToStr(timeout_queue->startTime, buf);
    uart_putstr(buf);
    uart_putstr("s, end excute time: ");
	unsignedlonglongToStr(get_excute_time(), buf);
    uart_putstr(buf);
	uart_putstr("s, duration time: ");
	unsignedlonglongToStr(timeout_queue->duration, buf);
    uart_putstr(buf);
    uart_putstr("s, message: ");
    uart_putstr(msg);
    uart_putstr("\n");
}
/*
*	timout_handle  
*   
*   1. if queue is empty, disable core timer and return
*   2. if queue not empty
*	   2.1 excute call back
*      2.2 point to next
*	3. if next not empty
*      next timeout = remainder time = alltime - currentTime
*   4. if next is empty, disable core timer
*   
*/
void timout_handle()
{
    if(timeout_queue == 0) 
	{
        core_timer_disable();
        return;
    }

	// excute callback print messgae
    timeout_queue->callback(timeout_queue->msg);
    timeout_queue->startTime = -1;
    timeout_queue = timeout_queue->next;
    
    if(timeout_queue != 0)
	{
        unsigned long long expire_time = timeout_queue->startTime + timeout_queue->duration - get_excute_time();
		set_next_timeout(expire_time);
    }
    else
        core_timer_disable();
    return;
}
