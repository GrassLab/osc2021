#include "timer.h"

timer_event timer_event_queue[20];

void init_timer_queue(){
    for(int i = 0; i < 20; ++i)
        timer_event_queue[i] = {.registerTime=-1, .exectime=-1, .msg[0]='\0', .next=nullptr};
}
void add_timer(void (*)(char *)func, int duration){

}
void timer_print_msg(char* msg){

}
void el1_timer_irq(){
    core_timer_disable();
    // core_timer_handler();
    // unsigned int second = get_current_timer_cnt() / TIME_FREQ;
    // uart_printint(second);
    // uart_puts("&&\r\n");
    // asm volatile ("msr cntp_tval_el0, %0" :: "r" (2 * TIME_FREQ));
    return;
}
void el0_timer_irq(){
    // core_timer_disable();
    // core_timer_handler();
    unsigned int second = get_current_timer_cnt() / TIME_FREQ;
    uart_printint(second);
    uart_puts("\r\n");
    asm volatile ("msr cntp_tval_el0, %0" :: "r" (2 * TIME_FREQ));
    return;
}