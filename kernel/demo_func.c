#include "uart.h"
#include "allocator.h"
#include "scheduler.h"
#include "cpio.h"
#include "string.h"
#include "vfs.h"

/* lab 3: Allocator*/
void lab3() {
    unsigned long test1 = (unsigned long)kmalloc(16);
    unsigned long test2 = (unsigned long)kmalloc(40);
    unsigned long test3 = (unsigned long)kmalloc(1<<12);
    unsigned long test4 = (unsigned long)kmalloc(1<<10);

    free_page(test3, 1<<12);
    free_page(test4, 1<<10);
    free_page(test2, 40);
    free_page(test1, 16);
}

/*lab5: thread*/
static void foo(){
    for(int i = 0; i < 10; ++i) {
        uart_puts("Thread id: ");
        uart_puts_int(current->id);
        uart_puts(" ,i = ");
        uart_puts_int(i);
        uart_puts("\n");
        delay(1000000);
        scheduler();
    }
    _exit();
}

void lab5_required_1(int N) {
    int flag = 0;
    for(int i = 0; i < N; ++i) { // N should > 2
        flag = create_thread(PF_KTHREAD, (unsigned long)&foo, 0, 0);
    }
    flag = create_thread(PF_KTHREAD, (unsigned long)&idle, 0, 0);
    while(1) scheduler();
}

void foo2() {
    char *argv[] = {"argv_test", "-o", "arg2", 0};
    _exec("argv_test.img", argv);
    scheduler();
}

void lab5_required_2() {
    int flag = create_thread(PF_KTHREAD, (unsigned long)&foo2, 0, 0);
    while(1) scheduler();
}

void lab6_required_1() {
    file* a = vfs_open("hello", O_CREATE);
    file* b = vfs_open("world", O_CREATE);
    if(vfs_write(a, "Hello ", 6) == 0) {
        uart_puts("write Hello error!\n");
    }
    
    if(vfs_write(b, "World!", 6) == 0) {
        uart_puts("write World! error\n");
    }
    
    vfs_close(a);
    vfs_close(b);
    
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    
    int sz = 0;
    char buf[100];
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf+sz, 100);
    
    buf[sz] = '\0';
    uart_puts(buf);
    uart_puts("\n");
}

void foo3() {
    char *argv[] = {"argv_test", "-o", "arg2", 0};
    _exec("file.img", argv);
    scheduler();
}

void lab6_required_2() {
    create_thread(PF_KTHREAD, (unsigned long)&foo3, 0, 0);
    while(1) scheduler();
}
