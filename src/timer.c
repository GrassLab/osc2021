#include "timer.h"

timer_event timer_event_buffer[20];
timer_event *timer_event_queue;
int event_buffer_idx;
unsigned long long TIME_FREQ;

void init_timer_queue(){
    for(int i = 0; i < 20; ++i){
        timer_event_buffer[i].registerTime = 0;
        timer_event_buffer[i].duration = 0;
        timer_event_buffer[i].next = nullptr;
    }
    timer_event_queue = nullptr;   
    event_buffer_idx = 0;
    TIME_FREQ = get_timer_freq();
}
void add_timer(void (*callback)(char *), char* args, int duration){
    unsigned long long current_cnt = get_current_timer_cnt();
    unsigned long long execTime = current_cnt + duration * TIME_FREQ;
    timer_event *prev = nullptr, *cur = nullptr;
    for(cur = timer_event_queue; cur != nullptr; prev = cur, cur = cur->next){
        if(cur->registerTime + cur->duration > execTime) break;
    }
    timer_event_buffer[event_buffer_idx].registerTime = current_cnt;
    timer_event_buffer[event_buffer_idx].duration = duration * TIME_FREQ;
    timer_event_buffer[event_buffer_idx].callback = callback;
    for(int i = 0;i < 20; ++i){
        timer_event_buffer[event_buffer_idx].args[i] = args[i];
        if(args[i] == '\0') break;
    }
   
    if(prev == nullptr && cur == nullptr) {
        timer_event_queue = &timer_event_buffer[event_buffer_idx];
        core_timer_enable();
        asm volatile ("msr cntp_tval_el0, %0" :: "r" (timer_event_queue->duration));
    }
    else if(prev != nullptr && cur == nullptr) prev->next = &timer_event_buffer[event_buffer_idx];
    else{
        timer_event_buffer[event_buffer_idx].next = cur;
        if(prev != nullptr) 
            prev->next = &timer_event_buffer[event_buffer_idx];
        else{
            timer_event_queue = &timer_event_buffer[event_buffer_idx];
            asm volatile ("msr cntp_tval_el0, %0" :: "r" (timer_event_queue->duration));
        }
           
    }
    //uart_printhex(timer_event_queue->registerTime);
    for(int i = 0; i < 20 && timer_event_buffer[event_buffer_idx].registerTime > 0; ++i){
        ++event_buffer_idx;
        //uart_printint(event_buffer_idx);
        if(event_buffer_idx == 20) event_buffer_idx = 0;
    }
    //uart_printint(event_buffer_idx);
    return;
}
void timer_print_msg(char* msg){
    unsigned long long current_cnt = get_current_timer_cnt();
    uart_puts("register time: ");
    uart_printint(timer_event_queue->registerTime / TIME_FREQ);
    uart_puts(", execute time: ");
    uart_printint(current_cnt / TIME_FREQ);
    uart_puts(", message: ");
    uart_puts(msg);
    uart_puts("\r\n");
    return;
}
void el1_timer_irq(){
    //core_timer_disable();
    if(timer_event_queue == nullptr) {
        core_timer_disable();
        return;
    }

    timer_event_queue->callback(timer_event_queue->args);
    timer_event_queue->registerTime = -1;
    unsigned long long dt = timer_event_queue->duration;
    timer_event_queue = timer_event_queue->next;
    
    if(timer_event_queue != nullptr){
        unsigned long long expire_time = timer_event_queue->duration + timer_event_queue->registerTime - get_current_timer_cnt();
        asm volatile ("msr cntp_tval_el0, %0" :: "r" (expire_time));
    }
    else
        core_timer_disable();
    return;
}
void el0_timer_irq(){
    unsigned int second = get_current_timer_cnt() / TIME_FREQ;
    uart_printint(second);
    uart_puts("\r\n");
    asm volatile ("msr cntp_tval_el0, %0" :: "r" (2 * TIME_FREQ));
    return;
}