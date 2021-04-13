#include "timer.h"

timer_event timer_event_queue[20];

void init_timer_queue(){

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