#include "include/irq.h"
#include "include/mini_uart.h"
#include "utils.h"
#include "include/cirq.h"
#include "include/csched.h"

struct irq_btm_task irq_btm_q[MAX_NR_BOTTOM];
int irq_btm_total = 0;

struct tqe *timerQueue = 0; // initialized to NUL
struct tqe timerPool[MAX_NR_TQE];
struct tqe *timerPool_h;

extern struct task *current;


void *core_timer_btm(void* arg)
{
    enable_irq();
    // uart_send_string("From core_timer_btm \r\n");
    // After bottom half finished, we have to disable interrupt.
    disable_irq();
    /* Critical section start */
    irq_btm_q_delete_max();
    btm_sched();
    /* Critical section end */
    return 0;
}

void do_core_timer_handler(void)
{
    // uart_send_string("From do_core_timer_handler: ");
    // uart_send_ulong(core_timer_get_sec());
    // uart_send_string(" seconds have passed.\r\n");
    set_core_timer(TICKS_FOR_ITR);
    if (--current->counter <= 0) {
        current->resched = 1;
        current->counter = current->priority;
    }
    tqe_decr(TICKS_FOR_ITR);
    /* After top half, we have to set bottom half task. */
    // irq_btm_q_insert(PRIORITY_TIMER, core_timer_btm);
}

/* btm_sched will only be called in 2 cases:
 * 1. After top half finished
 * 2. After bottom half finished
 * In case 1, interrupt is disabled by hardware
 * so it's safe to run "critical part" below.
 * In case 2, bottom half handler has to disable
 * interrupt before calling btm_sched(), so it's
 * safe too.
 */
void btm_sched()
{
    // max is dirty, means max task has once
    // been interrupted. This will only return
    // to case 1 caller. 
    if (!irq_btm_total || irq_btm_q[1].dirty)
        return;
    irq_btm_q[1].dirty = 1;
    void *(*btm_handler)(void*) = irq_btm_q[1].btm_handler;
    btm_handler(0);
}

#define CNTPSIRQ  0
#define CNTPNSIRQ 1
#define CNTHPIRQ  2
#define CNTVIRQ   3
#define MAILBOX0  4
#define MAILBOX1  5
#define MAILBOX2  6
#define MAILBOX3  7
#define GPU       8
#define PMU       9


void first_level_irq_handler(int type)
{
    switch (type) {
        case CNTPNSIRQ:
            do_core_timer_handler();
            break;
        case GPU:
            // level 2 has pending interrupt
            // uart_send_string("level1 GPU interrupt.\r\n");
            break;
        default:
            uart_send_string("Unknown first level irq.\r\n");
    }
}

void second_level_irq_handler(int type)
{

    switch (type) {
        case (AUX_IRQ):
            do_uart_handler();
            break;
        default:
            uart_send_string("Unknown second level irq.\r\n");
    }
}

void irq_handler(void)
{
    unsigned int second_level_irq_pend = get32(IRQ_PENDING_1);
    unsigned int first_level_irq_pend = get32(CORE0_INTERRUPT_SOURCE);

    /* Handle all pending first level irq.
     * https://github.com/raspberrypi/documentation/blob/master/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf
     * p.16 tells the CORE0_INTERRUPT_SOURCE
     */
    for (int i = 0; i < 32; ++i)
        if (first_level_irq_pend & (1 << i))
            first_level_irq_handler(i);

    /* Handle all pending second level irq.
     * BCM2837-ARM-Peripherals p.113 tells the interrupt table.
    */
    for (int i = 0; i < 32; ++i)
        if (second_level_irq_pend & (1 << i))
            second_level_irq_handler(i);

    // btm_sched();
}

int timerPool_init()
{
    for (int i = 0; i < MAX_NR_TQE - 1; ++i)
        timerPool[i].next = &timerPool[i+1];
    timerPool_h = &timerPool[0];
    return 0;
}

int tqe_release()
{
    struct tqe *tmp = timerQueue;
    timerQueue = timerQueue->next;
    tmp->next = timerPool_h;
    timerPool_h = tmp;
    return 0;
}

struct tqe *tqe_new()
{
    struct tqe *new;

    if (!(new = timerPool_h)){
        uart_send_string("Error: timerPool is empty\r\n");
        return 0; //null
    }
    timerPool_h = timerPool_h->next; // maintain timerPool_h
    return new;
}

int tqe_decr(int tick)
{
    if (!timerQueue) // No timer is counting down.
        return 0;
    if ((timerQueue->tick -= tick) <= 0) { // tick coun down to zero
        // Run time-up action
        timerQueue->action((void*)timerQueue->args);
        tqe_release();
    }
    return 0;
}

int tqe_add(unsigned int tick, void *(*action)(void *), void *args)
{
    struct tqe *new, *tmp, *last;

    new = tqe_new();
    // Set tqe
    new->next = 0; // NULL
    new->tick = tick;
    new->action = action;
    new->args = args;
    // for (int i = 0; i < 10 && ((char*)args)[i] != '\0'; ++i) // lab4demo
        // ((char*)(new->args))[i] = ((char*)args)[i];
    // new->proc = current;

    /* Find appropriate position for new */
    if (!timerQueue){
        timerQueue = new;
        return 0;
    }
    tmp = timerQueue;
    last = 0; // NULL
    while ( tmp && new->tick > tmp->tick) {
        new->tick -= tmp->tick;
        last = tmp;
        tmp = tmp->next;
    }
    /* Install new into timerQueue */
    if (!tmp) {// new is tail of timerQueue
        last->next = new;
    } else if (!last) { // new is head of timerQueue
        timerQueue = new;
        new->next = tmp;
        tmp->tick -= new->tick;
    } else {
        last->next = new;
        new->next = tmp;
        tmp->tick -= new->tick;
    }

    return 0;
}

void wakeup(void)
{
    ;
}

// void sleep(int duration)
// {
//     // return tqe_add(duration, wakeup);
//     ;
// }

int irq_btm_q_adjust(int idx)
{ // idx: 1 ~ (MAX_NR_BOTTOM - 1)
    struct irq_btm_task k = irq_btm_q[idx];
    int j = 2 * idx;
    while (j <= irq_btm_total) {
        if (j != irq_btm_total)
            if (irq_btm_q[j].priority < irq_btm_q[j+1].priority)
                j++;
        if (irq_btm_q[j].priority <= k.priority) {
            break;
        } else {
            irq_btm_q[j/2] = irq_btm_q[j];
            j *= 2;
        }
    }
    irq_btm_q[j/2] = k;
    return 0;
}

struct irq_btm_task irq_btm_q_delete_max()
{
    struct irq_btm_task max_task = irq_btm_q[1];
    irq_btm_q[1] = irq_btm_q[irq_btm_total];
    irq_btm_total--;
    irq_btm_q_adjust(1);
    return max_task;
}

int irq_btm_q_insert(int priority, void *(*btm_handler)(void*))
{
    if (irq_btm_total >= MAX_NR_BOTTOM - 1)
        return -1; // Not enough space
    irq_btm_total++;
    int j = irq_btm_total;
    while (j/2 >= 1) {
        if (irq_btm_q[j/2].priority >= priority)
            break; // father > you, so stop challenging.
        // Challenge successfully, so father comes down.
        irq_btm_q[j].priority = irq_btm_q[j/2].priority;
        irq_btm_q[j].dirty = irq_btm_q[j/2].dirty;
        irq_btm_q[j].btm_handler = irq_btm_q[j/2].btm_handler;
        j /= 2;
    }
    irq_btm_q[j].priority = priority;
    irq_btm_q[j].dirty = 0;
    irq_btm_q[j].btm_handler = btm_handler;
    return 0;
}

#define UART_BUF_SIZE 3
char uart_send_buf[UART_BUF_SIZE];
int uart_send_buf_in = 0;
int uart_send_buf_out = 0;
char uart_recv_buf[UART_BUF_SIZE];
int uart_recv_buf_idx = 0;
int uart_recv_buf_in = 0;
int uart_recv_buf_out = 0;
int recv_ready = 0;
extern struct wait_h *uartQueue;

/* AUX_MU_IIR_REG: peripheral p.13 */
void do_uart_handler()
{
    char ch;
    unsigned int iir, lsr;

    iir = get32(AUX_MU_IIR_REG);
    if ((iir & 0x6) == 0x2)
    { // Transmit holding register empty 
        lsr = get32(AUX_MU_LSR_REG);
        while ( lsr & 0xc0) {
            // 5th bit is set if the transmit FIFO can
            // accept at least one byte.
            if (uart_send_buf_in == uart_send_buf_out) {
                put32(AUX_MU_IER_REG, 1); //Only enable receive interrupts
                return;
            }
            ch = uart_send_buf[uart_send_buf_out];
            uart_send_buf_out = (uart_send_buf_out + 1) % UART_BUF_SIZE;
            put32(AUX_MU_IO_REG, ch);
        }
        if (uart_send_buf_in == uart_send_buf_out)
            put32(AUX_MU_IER_REG, 1); //Only enable receive interrupts

    }
    else if ((iir & 0x6) == 0x4)
    { // Receiver holds valid byte
        while (get32(AUX_MU_LSR_REG) & 0x1) {
            // The receive FIFO holds at least 1 symbol.
            if ((uart_recv_buf_in + 1) % UART_BUF_SIZE == uart_recv_buf_out)
                return;
            ch = get32(AUX_MU_IO_REG) & 0xFF;
            uart_send(ch);
            if (uartQueue->head) {
                uart_send_string("From do_uart_handler: A\r\n");
                uart_recv_buf[uart_recv_buf_in] = ch;
                uart_recv_buf_in = (uart_recv_buf_in + 1) % UART_BUF_SIZE;
            } else {
                uart_send_string("No wait\r\n");
                return;
            }
        }
        struct wait_args *wa = new_wait_args();
        if (uartQueue->head) {
            wa->old = uartQueue->head;
            wa->waitQueue = uartQueue;
            rm_from_queue((void*)wa);
        }
    }
    // else if ((iir & 0x6) == 0x4)
    // { // Receiver holds valid byte
    //     if (recv_ready)
    //         return;
    //     while (get32(AUX_MU_LSR_REG) & 0x1) {
    //     // if (get32(AUX_MU_LSR_REG) & 0x1) {
    //         // The receive FIFO holds at least 1 symbol.
    //         ch = get32(AUX_MU_IO_REG) & 0xFF;
    //         if (ch != '\r') {
    //             // uart_send_async(ch);
    //             uart_send(ch);
    //             uart_recv_buf[uart_recv_buf_idx++] = ch;
    //         } else {
    //             uart_recv_buf[uart_recv_buf_idx] = '\0';
    //             recv_ready = 1;
    //             uart_recv_buf_idx = 0;
    //         }
    //     }
    // }
}

void invalid_handler()
{
    uart_send_string("From invalid_handler\r\n");
    while (1);
}


int chk_sched()
{ // if interrupt is not nested && current->resched:
  //     schedule()
  // But now assume interrupt won't nest
    if (current->resched){
        current->resched = 0;
        schedule();
    }
    return 0;
}