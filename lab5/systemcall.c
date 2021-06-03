#include "include/systemcall.h"
#include "include/scheduler.h"
#include "include/uart.h"
#include "include/util.h"
#include "include/process.h"

int getpid() {
    return current->pid;
}

int uart_read(char buf[], int size) {
    int read_bytes = 0;
    for (; read_bytes < size; read_bytes++) {
        buf[read_bytes] = uart_get_char();
    }
    return read_bytes;
}

int uart_write(const char buf[], int size) {
    int send_bytes = 0;
    for (; send_bytes < size ; send_bytes++) {
        uart_send(buf[send_bytes]);
    }
    return send_bytes;
}

unsigned long svc_handler(unsigned long x0, unsigned long x1,
                 unsigned long x2, unsigned long x3,
                 unsigned long x4, unsigned long x5, unsigned long x6) {
    unsigned long ret;
    switch (x6) {   //x6 determine use which system call
        case SYS_GETPID:
            ret = getpid();
            break;
        case SYS_READ:
            ret = uart_read((char *)x0, (int)x1);
            break;
        case SYS_WRITE:
            ret = uart_write((const char *)x0, (int)x1);
            break;
        case SYS_EXEC:
            exec((const char*)x0, (char **const)x1);
            break;
        case SYS_EXIT:
            exit();
            break;
        case SYS_TIME:
            ret = get_time();
            break;
        case SYS_FORK:
            ret = fork();
            break;
        
        default:
            ret = -1;
            uart_put_str("system call undefine.\n");
    }
    return ret;
}